<template>
	<div>
	</div>
</template>

<script>
    "use strict";

    export default {
        props: {
            description: {type: Object, mandatory: true},
        },
        mounted() {
            this.$emit("link", this.description["link.url"]);
        },
		data: function() {
			return {
			}
		},
        methods: {
            getMetadata() {
                return {
                    name: "Link",
                    icon: "bzd-icon-link",
                    form: [
                        { type: "Input", name: "link.url", caption: "URL", placeholder: "https://www.google.com" },
                    ]
                }
            }
        }
    }
</script>

<style lang="scss">
	$bzdIconNames: link;
	@import "~[bzd]/icons.scss";
</style>
