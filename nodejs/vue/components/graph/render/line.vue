<template>
	<g>
		<path :d="path" class="path" fill="transparent" />
		<line
			v-for="(data, index) in series.coords"
			:x1="data[0]"
			:y1="data[1]"
			:x2="data[0]"
			:y2="data[1]"
			:class="getPointClass(index)"
		/>
	</g>
</template>

<script>
	export default {
		props: {
			series: { type: Object, required: true },
			selected: { type: Number, required: true },
		},
		computed: {
			path() {
				let path = "";
				for (let i = 0; i < this.series.coords.length; ++i) {
					const x = this.series.coords[i];
					const y = this.series.coords[i];

					path += i == 0 ? "M " + x + " " + y : " L " + x + " " + y;
				}
				return path;
			},
		},
		methods: {
			getPointClass(index) {
				return {
					circle: true,
					selected: index == this.selected,
				};
			},
		},
	};
</script>
