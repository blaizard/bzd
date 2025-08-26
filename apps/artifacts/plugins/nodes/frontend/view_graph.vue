<template>
	<div class="graph-container" style="width: 800px; height: 300px">
		<canvas ref="graph"></canvas>
	</div>
</template>

<script>
	import Chart from "chart.js/auto";
	import "chartjs-adapter-date-fns";
	import { bytesToString } from "#bzd/nodejs/utils/to_string.mjs";

	export default {
		props: {
			options: { mandatory: true, type: Object },
			// It should be formatted as { <name>: { "data": [[<time>, <value>], ...], "options": {} } }
			inputs: { mandatory: true, type: Object },
		},
		data: function () {
			// It's important to make sure chart is not reactive.
			// see: https://github.com/chartjs/Chart.js/issues/8970
			this.chart = null;
			return {};
		},
		watch: {
			datasets() {
				if (this.chart) {
					this.chart.data.datasets = this.datasets;
					this.chart.update("none"); // "none" suppress animation.
				}
			},
		},
		mounted() {
			const nopFormatter = (x) => x;
			const unitFormatter =
				{
					bytes: bytesToString,
					percent: (x) => x * 100 + "%",
					celsius: (x) => x + "°C",
				}[this.options.unit] || nopFormatter;

			this.chart = new Chart(this.$refs.graph, {
				type: "line",
				data: {
					datasets: [],
				},
				options: {
					responsive: true,
					maintainAspectRatio: false,
					plugins: {
						title: {
							display: true,
							text: this.options.title || "Missing title",
							font: {
								size: 20,
							},
						},
						tooltip: {
							mode: "index", // Show tooltip for all datasets at that index
							intersect: false, // Tooltip appears even if not directly hovering a point
							callbacks: {
								label: (item) => {
									if (unitFormatter === nopFormatter) {
										return item.formattedValue;
									}
									return unitFormatter(item.parsed.y) + " (" + item.formattedValue + ")";
								},
							},
						},
					},
					scales: {
						x: {
							type: "timeseries",
							title: {
								display: true,
								text: "Time",
							},
							time: {
								unit: "minute",
							},
						},
						y: {
							suggestedMin: this.options.min,
							suggestedMax: this.options.max,
							ticks: {
								callback: function (value, index, ticks) {
									return unitFormatter(value);
								},
							},
						},
					},
				},
			});
		},
		beforeUnmount() {
			if (this.chart) {
				this.chart.destroy();
				this.chart = null;
			}
		},
		computed: {
			datasets() {
				return Object.entries(this.inputs)
					.map(([name, input]) => {
						if (input.data.length > 0 && Array.isArray(input.data[0][1])) {
							const zippedData = input.data[0][1].map((_, index) => input.data.map(([t, v]) => [t, v[index]]));
							return zippedData.map((data, index) => ({
								label: name + "[" + index + "]",
								data: data,
							}));
						}
						return [
							{
								label: name,
								data: input.data.map(([t, v]) => [t, v]),
							},
						];
					})
					.flat();
			},
		},
		methods: {},
	};
</script>

<style lang="scss" scoped></style>
