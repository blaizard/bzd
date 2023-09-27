<template>
	<div>
		Hello
		<Plot :value="values"></Plot>
	</div>
</template>

<script>
	import Plot from "#bzd/nodejs/vue/components/graph/plot.vue";

	export default {
		components: {
			Plot,
		},
		data: function () {
			return {
				values: [],
				counter: 0,
			};
		},
		mounted() {
			this.updateValues();
			setInterval(() => {
				this.updateValues();
			}, 50);
		},
		methods: {
			generateSin() {
				return [...Array(100).keys()].map((index) => [index + this.counter, Math.sin((index + this.counter) / 10)]);
			},
			generateRandom() {
				return [...Array(100).keys()].map((index) => [index + this.counter, Math.random() * 2 - 1]);
			},
			updateValues() {
				++this.counter;
				this.values = [
					{
						caption: "Serie 1",
						values: this.generateSin(),
					},
					{
						caption: "Serie 2",
						values: this.generateRandom(),
					},
				];
			},
		},
	};
</script>
