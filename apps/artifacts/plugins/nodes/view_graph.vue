<template>
	<div class="graph-container" style="width: 800px; height: 300px">
		<canvas ref="graph"></canvas>
	</div>
</template>

<script>
	import Chart from "chart.js/auto";
	import "chartjs-adapter-date-fns";

	export default {
		props: {
			title: { mandatory: true, type: String },
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
					this.chart.update();
				}
			},
		},
		mounted() {
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
							text: this.title,
						},
						tooltip: {
							mode: "index", // Show tooltip for all datasets at that index
							intersect: false, // Tooltip appears even if not directly hovering a point
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
							beginAtZero: true,
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
				return Object.entries(this.inputs).map(([name, input]) => {
					return {
						label: name,
						data: input.data,
					};
				});
			},
		},
		methods: {},
	};
</script>

<style lang="scss" scoped></style>
