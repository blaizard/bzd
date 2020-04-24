<template>
	<div>
		jenkins component!
	</div>
</template>

<script>
    "use strict";

    export default {
        methods: {
            getMetadata() {
                return {
                    name: "Jenkins",
                    icon: "bzd-icon-jenkins",
                    form: [
                        { type: "Input", name: "jenkins.url", caption: "Jenkins URL", placeholder: "http://localhost:8080", width: 1 },
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
