<template>
	<g>
		<rect v-for="data in serie.coords"
				class="rectangle"
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
			boundingBox: {type: Object, required: true}
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
        }
    }
</script>
