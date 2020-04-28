<template>
	<div>
        <Plot :config="plotConfig" :value="plotValue"></Plot>
        {{ JSON.stringify(metadata) }}
	</div>
</template>

<script>
    "use strict";

    import Plot from "[bzd]/vue/components/graph/plot.vue"; 

    export default {
        components: {
            Plot
        },
        props: {
            metadata: {type: Object, mandatory: true}
        },
        computed: {
            plotConfig() {
                return {
                    showLegend: false,
                    showAxisX: false,
                    showAxisY: false,
                    showCursor: false,
                    paddingLeft: 5,
                    paddingRight: 5,
				    formatY: (y) => (Number(y) / 1000 / 60).toFixed(1) + " min",
                }
            },
            builds() {
                return this.metadata.builds || [];
            },
            plotValue() {
                const buildsReverse = [...this.builds].reverse();
                return ["processing", "success", "failure", "aborted", "unknown"].map((status) => {
                    return {
                        caption: status,
                        type: "bar",
                        values: buildsReverse.map((build, index) => [index, (build.status == status) ? build.duration : 0])
                    };
                });
            }
        },
        methods: {
            getMetadata() {
                return {
                    name: "Jenkins",
                    icon: "bzd-icon-jenkins",
                    form: [
                        { type: "Input", name: "jenkins.url", caption: "Jenkins URL", placeholder: "http://localhost:8080", width: 1 },
                        { type: "Input", name: "jenkins.user", caption: "User", width: 0.5 },
                        { type: "Input", name: "jenkins.token", caption: "Token", width: 0.5 },
                        { type: "Input", name: "jenkins.build", caption: "Build name", width: 0.5 },
                        { type: "Dropdown", name: "jenkins.branch", caption: "Branch", placeholder: "master", list: ["master"], edit: true, width: 0.5 },
                    ]
                }
            }
        }
    }
</script>

<style lang="scss">
	@import "~[bzd]/icons.scss";

    .bzd-icon-jenkins {
		@include defineIcon("jenkins.svg");
	}
</style>
