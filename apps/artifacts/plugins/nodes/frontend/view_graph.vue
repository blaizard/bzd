<template>
	<div class="graph-container">
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
			timeRange: { mandatory: true, type: Array },
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
			timeRange() {
				if (this.timeRange[0] && this.timeRange[1]) {
					this.chart.options.scales.x.min = this.timeRange[0];
					this.chart.options.scales.x.max = this.timeRange[1];
					this.chart.update("none"); // "none" suppress animation.
				}
			},
		},
		mounted() {
			const nopFormatter = (x) => x;
			const unitFormatter =
				{
					bytes: bytesToString,
					"bytes/s": (x) => bytesToString(x) + "/s",
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
					elements: {
						point: {
							radius: 1,
						},
					},
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
									const label = item.dataset.label || "";
									if (unitFormatter === nopFormatter) {
										return label + ": " + item.formattedValue;
									}
									return label + ": " + unitFormatter(item.parsed.y) + " (" + item.formattedValue + ")";
								},
							},
						},
						legend: {
							maxHeight: 30, // Hides if the legend takes 2 lines, this avoid shrinking the actual graph
							labels: {
								// Creates small round labels
								boxWidth: 12,
								boxHeight: 12,
								useBorderRadius: true,
								borderRadius: 6,
								font: {
									size: 12,
								},
							},
						},
					},
					scales: {
						x: {
							type: "time",
							title: {
								display: true,
								text: "Time",
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
						const label = this.options.inputs?.[name]?.name ?? name;
						if (input.data.length > 0 && Array.isArray(input.data[0][1])) {
							const zippedData = input.data[0][1].map((_, index) => input.data.map(([t, v]) => [t, v[index]]));
							return zippedData.map((data, index) => ({
								label: label + "[" + index + "]",
								data: data,
								spanGaps: false,
							}));
						}
						return [
							{
								label: label,
								data: input.data.map(([t, v]) => [t, v]),
								spanGaps: false,
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
