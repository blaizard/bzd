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
                        { type: "Input", name: "jenkins.name", caption: "Name", placeholder: "Jenkins...", width: 0.5 },
                        { type: "Dropdown", caption: "Type", width: 0.5 },
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
