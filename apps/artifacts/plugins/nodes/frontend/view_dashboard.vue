<template>
	<div class="view-dashboard">
		<Form :description="formDescription" v-model="options"></Form>
		<div class="components" v-loading="loading">
			<template v-for="dashboard in dashboards">
				<ViewGraph :inputs="dashboardInputs(dashboard)" :options="dashboard" :timeRange="timeRange"> </ViewGraph>
			</template>
		</div>
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import ViewGraph from "#bzd/apps/artifacts/plugins/nodes/frontend/view_graph.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import Utils from "#bzd/apps/artifacts/common/utils.mjs";
	import TimeseriesCollection from "#bzd/apps/artifacts/plugins/nodes/frontend/timeseries_collection.mjs";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.mjs";
	import { dateToDefaultString } from "#bzd/nodejs/utils/to_string.mjs";
	import { arrayFindCommonPrefix } from "#bzd/nodejs/utils/array.mjs";

	export default {
		mixins: [Base, Component],
		components: {
			ViewGraph,
			Form,
		},
		directives: {
			loading: DirectiveLoading,
		},
		data: function () {
			return {
				dashboards: [],
				// Timestamp difference between server and client.
				timestampDiff: 0,
				inputs: new TimeseriesCollection(),
				options: {
					interval: "Last 15 minutes",
				},
				timeout: null,
				periodMs: null,
			};
		},
		watch: {
			async "options.interval"() {
				await this.triggerFetch();
			},
			async dashboards() {
				await this.triggerFetch();
			},
		},
		mounted() {
			this.fetchDashboards();
		},
		beforeUnmount() {
			this.inputs.close();
		},
		computed: {
			intervalOptions() {
				return {
					"Last 5 minutes": async () => {
						await this.useLastPeriod(5 * 60 * 1000);
					},
					"Last 15 minutes": async () => {
						await this.useLastPeriod(15 * 60 * 1000);
					},
					"Last 30 minutes": async () => {
						await this.useLastPeriod(30 * 60 * 1000);
					},
					"Last 1 hour": async () => {
						await this.useLastPeriod(1 * 60 * 60 * 1000);
					},
					"Last 3 hours": async () => {
						await this.useLastPeriod(3 * 60 * 60 * 1000);
					},
					"Last 6 hours": async () => {
						await this.useLastPeriod(6 * 60 * 60 * 1000);
					},
					"Last 12 hours": async () => {
						await this.useLastPeriod(12 * 60 * 60 * 1000);
					},
					"Last 24 hours": async () => {
						await this.useLastPeriod(24 * 60 * 60 * 1000);
					},
					"Last 2 days": async () => {
						await this.useLastPeriod(2 * 24 * 60 * 60 * 1000);
					},
					"Last 7 days": async () => {
						await this.useLastPeriod(7 * 24 * 60 * 60 * 1000);
					},
					"Last 30 days": async () => {
						await this.useLastPeriod(30 * 24 * 60 * 60 * 1000);
					},
					"Last 6 months": async () => {
						await this.useLastPeriod(6 * 30 * 24 * 60 * 60 * 1000);
					},
					"Last 1 year": async () => {
						await this.useLastPeriod(365 * 24 * 60 * 60 * 1000);
					},
					"Last 2 years": async () => {
						await this.useLastPeriod(2 * 365 * 24 * 60 * 60 * 1000);
					},
					"Last 5 years": async () => {
						await this.useLastPeriod(5 * 365 * 24 * 60 * 60 * 1000);
					},
				};
			},
			dashboardEndpoint() {
				const [volume, ...key] = this.pathList;
				return "/x/" + encodeURIComponent(volume) + "/@dashboards" + Utils.keyToPath(key);
			},
			endpoint() {
				const [volume, uid, ..._] = this.pathList;
				return "/x/" + encodeURIComponent(volume) + "/" + encodeURIComponent(uid);
			},
			// Gather all inputs to gather.
			inputsKeys() {
				return [
					...new Set(
						this.dashboards
							.map((dashboard) => {
								return Object.keys(dashboard.inputs);
							})
							.flat(),
					),
				];
			},
			timeRange() {
				const [_, timestampNewest] = this.inputs.timeRange;
				if (timestampNewest === null || this.periodMs === null) {
					return [null, null];
				}
				return [timestampNewest - this.periodMs, timestampNewest];
			},
			timeRangeString() {
				const [timestampOldest, timestampNewest] = this.timeRange;
				if (timestampOldest === null && timestampNewest === null) {
					return "";
				}
				const result = [
					timestampOldest ? dateToDefaultString(timestampOldest) : "?",
					timestampNewest ? dateToDefaultString(timestampNewest) : "?",
				];
				return result.join(" - ");
			},
			formDescription() {
				return [
					{
						type: "Dropdown",
						name: "interval",
						list: Object.keys(this.intervalOptions),
					},
					{
						type: "Message",
						value: this.dashboards.length > 0 ? this.timeRangeString : "No dashboards",
						align: "center",
					},
				];
			},
		},
		methods: {
			async useLastPeriod(periodMs) {
				this.loading = true;
				try {
					const nbSamples = 800;

					this.periodMs = periodMs;
					this.inputs.reset({ periodLimit: this.periodMs });

					// Look for the latest sample and fetch data from there.
					// This takes care of sample with time unsyncrhonized with the server.
					let data = await this.fetchData({ count: 1 });
					this.inputs.add(data);
					const timestampNewest = this.inputs.timeRange[1];
					if (timestampNewest !== null) {
						data = await this.fetchData({
							before: timestampNewest,
							after: timestampNewest - this.periodMs,
							count: nbSamples,
						});
						this.inputs.add(data);
					}

					// Adjust the refresh period to match the sampling of the graph.
					// When we want real time (>=15min) we grasp as many samples as we can,
					// otherwise we sample.
					const refreshPeriodMs = Math.max(periodMs / nbSamples, 1000);
					const refreshCount = periodMs <= 15 * 60 * 1000 ? nbSamples : 1;

					console.log(refreshPeriodMs, refreshCount);

					this.inputs.refreshPeriodically(async ([_, timestampNewest]) => {
						return await this.fetchData({ after: timestampNewest, count: refreshCount });
					}, refreshPeriodMs);
				} finally {
					this.loading = false;
				}
			},
			async triggerFetch() {
				if (this.dashboards && this.options.interval in this.intervalOptions) {
					await this.intervalOptions[this.options.interval]();
				}
			},
			dashboardInputs(dashboard) {
				let inputs = {};
				for (const [input, options] of Object.entries(dashboard.inputs)) {
					if (this.inputs.has(input)) {
						inputs[input] = {
							data: this.inputs.get(input),
							options: options || {},
						};
					}
				}
				return inputs;
			},
			timeToServer(timestamp) {
				return timestamp + this.timestampDiff;
			},
			async fetchDashboards() {
				await this.handleSubmit(
					async () => {
						// Get the dashboard and approximate the time difference between the server and the client.
						const t1 = Utils.timestampMs();
						const result = await this.requestBackend(this.dashboardEndpoint, {
							method: "get",
							expect: "json",
						});
						const t4 = Utils.timestampMs();
						const networkDelay = (t4 - t1) / 2; // Time it took to receive the response.
						this.timestampDiff = result.timestamp + networkDelay - t4;

						// Update the input name for the inputs of the dashboard.
						for (const dashboard of result.dashboards) {
							const inputs = Object.entries(dashboard.inputs).map(([path, options]) => [
								path,
								options,
								Utils.pathToKey(path),
							]);
							const commonPrefixLength = arrayFindCommonPrefix(...inputs.map(([_1, _2, key]) => key)).length;
							dashboard.inputs = Object.fromEntries(
								inputs.map(([path, options, key]) => {
									options = Object.assign(
										{
											name: inputs.length == 1 ? key.at(-1) : key.slice(commonPrefixLength).join("."),
										},
										options,
									);
									return [path, options];
								}),
							);
						}
						this.dashboards = result.dashboards;
					},
					{ updateLoading: false },
				);
			},
			async fetchData({ before = null, after = null, count = 800 } = {}) {
				return await this.handleSubmit(
					async () => {
						const query = Object.fromEntries(
							Object.entries({
								include: this.inputsKeys.join(","),
								metadata: 1,
								count: count,
								before: before,
								after: after,
							}).filter(([_, v]) => v !== null),
						);

						const result = await this.requestBackend(this.endpoint, {
							method: "get",
							query: query,
							expect: "json",
						});
						let inputs = {};
						for (const [key, data] of result.data) {
							inputs[Utils.keyToPath(key)] = data;
						}
						return inputs;
					},
					{ updateLoading: false },
				);
			},
		},
	};
</script>

<style lang="scss" scoped>
	.components {
		display: flex;
		flex-direction: row;
		flex-wrap: wrap;
	}
</style>
