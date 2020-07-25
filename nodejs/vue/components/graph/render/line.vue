<template>
	<g>
		<path :d="path" class="path" fill="transparent" />
		<line
			v-for="(data, index) in serie.coords"
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
			serie: { type: Object, required: true },
			selected: { type: Number, required: true },
		},
		computed: {
			path() {
				let path = "";
				for (let i = 0; i < this.serie.coords.length; ++i) {
					const x = this.serie.coords[i];
					const y = this.serie.coords[i];

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
