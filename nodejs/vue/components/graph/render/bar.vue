<template>
	<g ref="container">
		<rect
			v-for="(data, key) in series.coords"
			:class="getPointClass(key)"
			:x="data[0] - barWidth / 2"
			:y="data[1]"
			:width="barWidth"
			:height="boundingBox.bottom > data[1] ? boundingBox.bottom - data[1] : 0"
		/>
	</g>
</template>

<script>
	export default {
		props: {
			series: { type: Object, required: true },
			boundingBox: { type: Object, required: true },
			selected: { type: Number, required: true },
		},
		watch: {
			selected: {
				immediate: true,
				handler(value) {
					if (value != -1) {
						const elt = this.$refs.container.childNodes[value];
						this.series.tooltip(elt);
					}
				},
			},
		},
		computed: {
			barWidth() {
				let minDiffX = this.boundingBox.right - this.boundingBox.left;
				let prevX = this.series.coords[0][0];
				for (let i = 1; i < this.series.coords.length; ++i) {
					minDiffX = Math.min(minDiffX, this.series.coords[i][0] - prevX);
				}
				return Math.max(minDiffX - 2, 1);
			},
		},
		methods: {
			getPointClass(index) {
				return {
					rectangle: true,
					selected: index == this.selected,
				};
			},
		},
	};
</script>
