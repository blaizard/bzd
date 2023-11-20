<template>
	<text ref="text" :x="x" :y="y" :dy="dy" :text-anchor="computedHAlign">
		<slot></slot>
	</text>
</template>

<script>
	/**
	 * Print a text at specific coordinates, where x and y are the top-left anchor coordinates.
	 * This is needed as SVG text don't support yet alignment-baseline
	 */
	export default {
		props: {
			x: { type: String || Number, required: true },
			y: { type: String || Number, required: true },
			hAlign: { type: String, required: false, default: "left" },
		},
		data: function () {
			return {
				dy: 0,
			};
		},
		computed: {
			computedHAlign() {
				switch (this.hAlign) {
					case "left":
						return "start";
					case "middle":
						return "middle";
					case "right":
						return "end";
				}
			},
		},
		watch: {
			y: {
				immediate: true,
				async handler(y) {
					await this.$nextTick();
					this.dy += parseFloat(y) - this.$refs.text.getBBox().y;
				},
			},
		},
	};
</script>
