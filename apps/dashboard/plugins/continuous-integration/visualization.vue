<template>
	<div>
        <div v-if="isValid">Last build: {{ lastBuildDate }}</div>
        <div v-if="isValid">Buid: {{ buildPerWeek }}/week</div>
        <div v-if="isValid">Reliability: {{ buildReliability }}%</div>
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
            },
            lastBuildDate() {
                const date = new Date(this.builds[0].timestamp);
                return date;
            },
            isValid() {
                return (this.builds.length > 0);
            },
            periodDurationMs() {
                return this.builds[0].timestamp - this.builds[this.builds.length - 1].timestamp;
            },
            buildPerWeek() {
                const nbWeeks = this.periodDurationMs / (7 * 24 * 60 * 60 * 1000);
                return Math.round(this.builds.length / nbWeeks);
            },
            buildReliability() {
                this.builds.filter(() => {});
                const nbSuccessfullBuilds = this.builds.reduce((total, build) => {
                    return total + ((build.status == "success") ? 1 : 0);
                }, 0);
                return nbSuccessfullBuilds / this.builds;
            }
        },
        methods: {
            getMetadata() {
                return {
                    name: "Continuous Integration",
                    icon: "bzd-icon-continuous-integration",
                    form: [
                    ]
                }
            }
        }
    }
</script>

<style lang="scss">
	@import "~[bzd]/icons.scss";

    .bzd-icon-continuous-integration {
		@include defineIcon("continuous-integration.svg");
	}
</style>
