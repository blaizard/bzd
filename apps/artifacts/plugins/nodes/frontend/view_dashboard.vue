<template>
	<div class="view-dashboard">
		<Form :description="formDescription" v-model="options"></Form>
		<div class="components" v-loading="loading" ref="componentContainer">
			<div :class="dashboardClass(dashboard)" v-for="dashboard in dashboards">
				<h2 class="dashboard-component-title">{{ dashboard.title || "Missing title" }}</h2>
				<ViewGraph
					v-if="dashboard.type == 'linear'"
					:inputs="dashboardInputs(dashboard)"
					:options="dashboard"
					:timeRange="timeRange"
					class="dashboard-component-graph"
				>
				</ViewGraph>
				<div v-else class="dashboard-component-unsupported">Unsupported graph type "{{ dashboard.type }}".</div>
			</div>
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
	import LocalStorage from "#bzd/nodejs/core/localstorage.mjs";
	import Lock from "#bzd/nodejs/core/lock.mjs";

	const optionsStorageKey = "bzd-artifacts-plugin-nodes-view-dashboard-options";

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
				viewport: {
					dashboards: [],
					width: 0,
				},
				// Timestamp difference between server and client.
				timestampDiff: 0,
				// Cached information for the getTimestamp function.
				getTimestampCache: null,
				inputs: new TimeseriesCollection(),
				options: {
					interval: "Last 15 minutes",
				},
				timeout: null,
				periodMs: null,
				lock: new Lock(),
				viewportUpdatedTimeout: null,
			};
		},
		watch: {
			async triggerConditionsChange() {
				await this.triggerFetch();
			},
			options() {
				LocalStorage.setSerializable(optionsStorageKey, this.options);
			},
		},
		async mounted() {
			try {
				this.options = LocalStorage.getSerializable(optionsStorageKey, this.options);
			} catch (e) {
				// ignore.
			}
			await this.fetchDashboards();
			this.viewportUpdated();
			window.addEventListener("scroll", this.handleScroll);
		},
		beforeUnmount() {
			window.removeEventListener("scroll", this.handleScroll);
			clearTimeout(this.viewportUpdatedTimeout);
			this.inputs.close();
		},
		computed: {
			// Conditions that can trigger a new data fetch.
			triggerConditionsChange() {
				return [this.dashboards, this.options.interval, this.viewport];
			},
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
					return "...";
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
			handleScroll() {
				clearTimeout(this.viewportUpdatedTimeout);
				this.viewportUpdatedTimeout = setTimeout(this.viewportUpdated, 100);
			},
			viewportUpdated() {
				const elements = this.$refs.componentContainer.children;
				let viewport = {
					dashboards: [],
					width: 0,
				};
				[...elements]
					.filter((element) => element.classList.contains("dashboard-component"))
					.forEach((element, index) => {
						const rect = element.getBoundingClientRect();
						if (rect.bottom < 0 || window.innerHeight < rect.top) {
							return;
						}
						viewport.width = Math.max(viewport.width, rect.width);
						viewport.dashboards.push(this.dashboards[index]);
					});
				// Update only if needed.
				if (
					viewport.width != this.viewport.width ||
					!viewport.dashboards.every((d, index) => d === this.viewport.dashboards[index])
				) {
					this.viewport = viewport;
				}
			},
			/// Get information about the timestamp of the data sets.
			///
			/// This includes the newest timestamp and the timestamp diff between the client and the server.
			async getTimestamp() {
				if (this.getTimestampCache === null) {
					const timestampBefore = Utils.timestampMs();
					const data = await this.fetchData({ count: 1, all: true });
					const timestampAfter = Utils.timestampMs();
					const timestampClient = (timestampAfter + timestampBefore) / 2;

					// Get the newest timestamp.
					const inputs = new TimeseriesCollection();
					inputs.add(data);
					const timestampNewest = inputs.timeRange[1];
					inputs.close();

					this.getTimestampCache = {
						client: timestampClient,
						server: timestampNewest,
					};
				}

				// Return a tuple containing the current newest timestamp and the diff between the client and remote.
				if (this.getTimestampCache.server === null) {
					return [null, null];
				}
				const elapsedTime = Utils.timestampMs() - this.getTimestampCache.client;
				return [
					this.getTimestampCache.server + elapsedTime,
					this.getTimestampCache.server - this.getTimestampCache.client,
				];
			},
			async useLastPeriod(periodMs) {
				this.loading = true;
				try {
					const [timestampNewest, timestampDiff] = await this.getTimestamp();

					const nbSamples = Math.max(Math.round(this.viewport.width / 2), 100);
					this.periodMs = periodMs;
					this.inputs.reset({ periodLimit: this.periodMs });

					if (timestampNewest !== null) {
						const data = await this.fetchData({
							before: timestampNewest,
							after: timestampNewest - this.periodMs,
							count: nbSamples,
							all: false,
						});
						this.inputs.add(data);

						// Adjust the refresh period to match the sampling of the graph.
						const refreshPeriodMs = Math.max(periodMs / nbSamples, 1000);
						this.inputs.refreshPeriodically(async ([_, timestampNewestLocal]) => {
							const timestampNewestRemote = Math.max(Utils.timestampMs() + timestampDiff, timestampNewestLocal + 1);
							const periodRequestedMs = timestampNewestRemote - timestampNewestLocal;
							const count = Math.round((periodRequestedMs * nbSamples) / periodMs);
							if (count) {
								return await this.fetchData({
									after: timestampNewestLocal,
									before: timestampNewestRemote,
									count: count,
									sampling: "newest",
									all: false,
								});
							}
						}, refreshPeriodMs);
					}
				} finally {
					this.loading = false;
				}
			},
			// Gather all inputs to gather from the list of dashboards.
			inputsKeysFromDashboards(dashboards) {
				return [
					...new Set(
						dashboards
							.map((dashboard) => {
								return Object.keys(dashboard.inputs);
							})
							.flat(),
					),
				];
			},
			async triggerFetch() {
				if (this.dashboards && this.options.interval in this.intervalOptions && this.viewport.dashboards.length > 0) {
					await this.lock.acquire(async () => {
						await this.intervalOptions[this.options.interval]();
					});
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
			dashboardClass(dashboard) {
				return {
					"dashboard-component": true,
					"dashboard-component-medium": true,
				};
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
			async fetchData({ before = null, after = null, count = 800, sampling = null, all = false } = {}) {
				return await this.handleSubmit(
					async () => {
						const query = Object.fromEntries(
							Object.entries({
								include: this.inputsKeysFromDashboards(all ? this.dashboards : this.viewport.dashboards).join(","),
								metadata: 1,
								count: count,
								before: before,
								after: after,
								sampling: sampling,
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
		container-type: inline-size;

		.dashboard-component-title {
			width: 100%;
			height: 30px;
			line-height: 30px;
			font-size: 20px;
			margin: 0;
			text-align: center;
			text-overflow: ellipsis;
			overflow: hidden;
			white-space: nowrap;
		}

		.dashboard-component-graph,
		.dashboard-component-unsupported {
			width: 100%;
			height: calc(100% - 30px);
		}

		.dashboard-component-medium {
			width: 100%;
			height: 300px;
		}

		@container (width >= 800px) {
			.dashboard-component-medium {
				width: 50%;
			}
		}
	}
</style>
