<template>
	<div class="tooltip-content">
		<button v-tooltip="{ data: 'West', position: 'w' }">West</button>
		<button v-tooltip="{ data: 'North', position: 'n' }">North</button>
		<button v-tooltip="{ data: 'East', position: 'e' }">East</button>
		<button v-tooltip="{ data: 'South', position: 's' }">South</button>
	</div>
</template>

<script>
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";

	export default {
		directives: {
			tooltip: DirectiveTooltip,
		},
		data() {
			return {};
		},
		computed: {},
	};
</script>

<style lang="scss" scoped>
	.tooltip-content {
		padding: 200px;
	}
</style>
