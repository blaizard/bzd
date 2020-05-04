<template>
	<g ref="container">
		<rect v-for="data, key in serie.coords"
				:class="getPointClass(key)"
				:x="data[0] - barWidth / 2"
				:y="data[1]"
				:width="barWidth"
				:height="(boundingBox.bottom > data[1]) ? (boundingBox.bottom - data[1]) : 0" />
	</g>
</template>

<script>
"use strict";

export default {
	props: {
		serie: {type: Object, required: true},
		boundingBox: {type: Object, required: true},
		selected: {type: Number, required: true}
	},
	watch: {
		selected: {
			immediate: true,
			handler(value) {
				if (value != -1) {
					const elt = this.$refs.container.childNodes[value];
					this.serie.tooltip(elt);
				}
			}
		}
	},
	computed: {
		barWidth() {
			let minDiffX = this.boundingBox.right - this.boundingBox.left;
			let prevX = this.serie.coords[0][0];
			for (let i = 1; i<this.serie.coords.length; ++i) {
				minDiffX = Math.min(minDiffX, this.serie.coords[i][0] - prevX);
			}
			return Math.max(minDiffX - 2, 1);
		}
	},
	//tooltipFromCoords(coords.x, coords.y, "hello");
	methods: {
		getPointClass(index) {
			return {
				"rectangle": true,
				"selected": (index == this.selected)
			};
		}
	}
};
</script>
