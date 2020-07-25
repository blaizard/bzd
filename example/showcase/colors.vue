<template>
	<div class="colors">
		<div v-for="(color, name) in colors" :key="name" :style="setStyle(color)">
			{{ name }}
		</div>
	</div>
</template>

<script>
	import Colors from "bzd-style/css/colors.scss";

	export default {
		data: function () {
			return {};
		},
		computed: {
			colors() {
				return Colors;
			},
		},
		methods: {
			setStyle(color) {
				return "background-color: " + color + ";";
			},
		},
	};
</script>

<style lang="scss" scoped>
	.colors {
		display: flex;
		flex-flow: nowrap;

		> * {
			flex-grow: 1;
			writing-mode: vertical-rl;
			text-orientation: upright;
			height: 90vh;
		}
	}
</style>
