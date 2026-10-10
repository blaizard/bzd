<template>
	<div class="graph-container">
		<canvas ref="graph"></canvas>
	</div>
</template>

<script>
	import Chart from "chart.js/auto";
	import "chartjs-adapter-date-fns";
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import TimeseriesCollection from "#bzd/apps/artifacts/plugins/nodes/frontend/timeseries_collection.js";
	import Utils from "#bzd/apps/artifacts/common/utils.js";
	import { UCUMToString } from "#bzd/nodejs/utils/to_string.js";
	import ExceptionFactory from "#bzd/nodejs/core/exception.js";

	const Exception = ExceptionFactory("apps", "plugin", "nodes");
	const refreshPeriodMs = 3000;

	export default {
		mixins: [Base, Component],
		props: {
			options: { mandatory: true, type: Object },
			endpoint: { mandatory: true, type: String },
			timeRange: { mandatory: true, type: Array },
			configuration: { type: Object, default: () => ({}) },
		},
		emits: ["fetch", "timestamp", "update:configuration"],
		data: function () {
			// It's important to make sure chart is not reactive.
			// see: https://github.com/chartjs/Chart.js/issues/8970
			this.chart = null;
			return {
				collection: new TimeseriesCollection(),
				fetching: false,
				liveTimer: null,
			};
		},
		watch: {
			datasets() {
				if (this.chart) {
					this.chart.data.datasets = this.datasets;
					this.chart.update("none"); // "none" suppress animation.
				}
			},
			timeRange(value, previous) {
				this.applyTimeRange();
				const span = (range) => (range[0] === null || range[1] === null ? null : range[1] - range[0]);
				if (span(value) !== span(previous)) {
					this.loadInitial();
				}
			},
		},
		async mounted() {
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
										const formatted = this.options.unit ? UCUMToString(item.parsed.y, this.options.unit) : undefined;
										if (formatted) {
											return label + ": " + formatted + " (" + item.formattedValue + ")";
										}
										return label + ": " + item.formattedValue;
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
									callback: (value, index, ticks) => {
										return this.options.unit ? UCUMToString(value, this.options.unit, value) : value;
									},
								},
							},
						},
					},
				}),
			);
			this.applyTimeRange();
			await this.loadInitial();
			this.liveTimer = setInterval(async () => {
				await this.fetchLatest();
			}, refreshPeriodMs);
		},
		beforeUnmount() {
			clearInterval(this.liveTimer);
			this.collection.close();
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
				return Object.entries(this.collection.data)
					.map(([name, input]) => {
						const label = this.options.inputs?.[name]?.name ?? name;
						if (input.length > 0 && Array.isArray(input[0][1])) {
							const zippedData = input[0][1].map((_, index) => input.map(([t, v]) => ({ x: t / 1000, y: v[index] })));
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
								data: input.map(([t, v]) => ({ x: t / 1000, y: v })),
								spanGaps: false,
							}),
						];
					})
					.flat();
			},
		},
		methods: {
			async loadInitial() {
				if (this.fetching || this.timeRange[0] === null || this.timeRange[1] === null) {
					return;
				}
				this.fetching = true;
				try {
					const count = Math.max(Math.round(this.$el.clientWidth / 2), 100);
					this.collection.reset({ periodLimit: this.timeRange[1] - this.timeRange[0] });
					this.collection.add(
						await this.fetchData({ before: this.timeRange[1], after: this.timeRange[0], count: count }),
					);
					this.applyTimeRange();
					this.emitTimestamp();
				} finally {
					this.fetching = false;
				}
			},
			async fetchLatest() {
				if (this.fetching) {
					return;
				}
				const [, timestampNewest] = this.collection.timeRange;
				if (timestampNewest === null) {
					return;
				}
				this.fetching = true;
				try {
					this.collection.add(await this.fetchData({ after: timestampNewest, count: 100, sampling: "newest" }));
					this.applyTimeRange();
					this.emitTimestamp();
				} finally {
					this.fetching = false;
				}
			},
			async fetchData({ count, before = null, after = null, sampling = null }) {
				this.$emit("fetch", refreshPeriodMs);
				return await this.handleSubmit(
					async () => {
						const query = Object.fromEntries(
							Object.entries({
								include: Object.keys(this.options.inputs).join(","),
								metadata: 1,
								count: count,
								before: before,
								after: after,
								sampling: sampling,
							}).filter(([_, value]) => value !== null),
						);
						const result = await this.requestBackend(this.endpoint, {
							method: "get",
							query: query,
							expect: "json",
						});
						return Object.fromEntries(result.data.map(([key, data]) => [Utils.keyToPath(key), data]));
					},
					{ updateLoading: false },
				);
			},
			emitTimestamp() {
				const [, timestampNewest] = this.collection.timeRange;
				if (timestampNewest !== null) {
					this.$emit("timestamp", timestampNewest);
				}
			},
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
			applyTimeRange() {
				if (this.chart && this.timeRange[0] && this.timeRange[1]) {
					// Extend the window to include the newest fetched data, otherwise newly
					// arrived samples would be clipped until the next time range update.
					const [, timestampNewest] = this.collection.timeRange;
					const timestampMax =
						timestampNewest === null ? this.timeRange[1] : Math.max(this.timeRange[1], timestampNewest);
					this.chart.options.scales.x.min = this.timeRange[0] / 1000;
					this.chart.options.scales.x.max = timestampMax / 1000;
					this.chart.update("none"); // "none" suppress animation.
				}
			},
		},
	};
</script>

<style lang="scss" scoped></style>
