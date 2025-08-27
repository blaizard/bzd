<template>
	<div class="view-dashboard">
		<Form :description="formDescription" v-model="options"></Form>
		<div class="components" v-loading="loading">
			<template v-for="dashboard in dashboards">
				<ViewGraph :inputs="dashboardInputs(dashboard)" :options="dashboard"> </ViewGraph>
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
			timeRangeString() {
				const [timestampOldest, timestampNewest] = this.inputs.timeRange;
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
						value: this.timeRangeString,
						align: "center",
					},
				];
			},
		},
		methods: {
			async useLastPeriod(periodMs) {
				this.loading = true;
				try {
					this.inputs.reset({ periodLimit: periodMs });
					const now = this.timeToServer(Utils.timestampMs());
					const data = await this.fetchData({ before: now, after: now - periodMs });
					this.inputs.add(data);
					this.inputs.refreshPeriodically(async ([_, timestampNewest]) => {
						return await this.fetchData({ after: timestampNewest });
					}, 1000);
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
						const result = await this.requestBackend(this.dashboardEndpoint, {
							method: "get",
							expect: "json",
						});
						this.dashboards = result.dashboards;
						this.timestampDiff = result.timestamp - Utils.timestampMs();
					},
					{ updateLoading: false },
				);
			},
			async fetchData({ before = null, after = null }) {
				return await this.handleSubmit(
					async () => {
						const query = Object.fromEntries(
							Object.entries({
								include: this.inputsKeys.join(","),
								metadata: 1,
								count: 100,
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
