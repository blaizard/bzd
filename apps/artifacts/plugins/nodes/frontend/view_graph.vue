<template>
	<div class="graph-container">
		<canvas ref="graph"></canvas>
	</div>
</template>

<script>
	import Chart from "chart.js/auto";
	import "chartjs-adapter-date-fns";
	import { bytesToString } from "#bzd/nodejs/utils/to_string.mjs";
	import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

	const Exception = ExceptionFactory("apps", "plugin", "nodes");

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

			this.chart = new Chart(
				this.$refs.graph,
				this.adaptConfig({
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
							line: {
								borderWidth: 2,
							},
						},
						plugins: {
							title: {
								display: false,
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
				}),
			);
		},
		beforeUnmount() {
			if (this.chart) {
				this.chart.destroy();
				this.chart = null;
			}
		},
		computed: {
			graphType() {
				return this.options.type;
			},
			datasets() {
				return Object.entries(this.inputs)
					.map(([name, input]) => {
						const label = this.options.inputs?.[name]?.name ?? name;
						if (input.data.length > 0 && Array.isArray(input.data[0][1])) {
							const zippedData = input.data[0][1].map((_, index) =>
								input.data.map(([t, v]) => ({ x: t, y: v[index] })),
							);
							return zippedData.map((data, index) =>
								this.adaptDataset({
									label: label + "[" + index + "]",
									data: data,
									spanGaps: false,
								}),
							);
						}
						return [
							this.adaptDataset({
								label: label,
								data: input.data.map(([t, v]) => ({ x: t, y: v })),
								spanGaps: false,
							}),
						];
					})
					.flat();
			},
		},
		methods: {
			adaptDataset(dataset) {
				switch (this.graphType) {
					case "bar":
						return Object.assign(dataset, {
							barThickness: 5,
						});
				}
				return dataset;
			},
			adaptConfig(config) {
				switch (this.graphType) {
					case "linear":
						return Object.assign(config, {
							type: "line",
						});
					case "bar":
						return Object.assign(config, {
							type: "bar",
						});
					default:
						Exception.unreachable("Unsupported graph type: '{}'.", this.graphType);
				}
			},
		},
	};
</script>

<style lang="scss" scoped></style>
