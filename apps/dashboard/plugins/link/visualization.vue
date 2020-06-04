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
		};
	}
};
</script>

<style lang="scss">
	@use "bzd/icons.scss" with (
        $bzdIconNames: link
    );
</style>
