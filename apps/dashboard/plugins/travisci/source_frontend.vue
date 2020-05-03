<script>
    "use strict";

    export default {
        methods: {
            getMetadata() {
                return {
                    name: "Travis CI",
                    icon: "bzd-icon-travisci",
                    form: [
                        { type: "Input", name: "travisci.repository", caption: "Repository", placeholder: "For example, blaizard/cpp-async", width: 0.5 },
                        { type: "Input", name: "travisci.token", caption: "Token", width: 0.5 },
                    ],
                    defaultValue: {
                        "visualization.type": "continuous-integration",
                    }
                }
            }
        }
    }
</script>

<style lang="scss">
	@import "~[bzd]/icons.scss";

    .bzd-icon-travisci {
		@include defineIcon("travisci.svg");
	}
</style>
