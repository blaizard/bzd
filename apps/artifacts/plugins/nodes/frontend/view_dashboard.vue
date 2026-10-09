<template>
	<div class="view-dashboard">
		<div class="tags" v-if="availableTags.length > 0">
			<button class="tag" :class="{ active: selectedTag === null }" @click="selectTag(null)">All</button>
			<button
				class="tag"
				v-for="tag in availableTags"
				:key="tag"
				:class="{ active: selectedTag === tag }"
				@click="selectTag(tag)"
			>
				{{ tag }}
			</button>
		</div>
		<Form :description="formDescription" v-model="options"></Form>
		<div class="components" v-loading="loading" ref="componentContainer">
			<div
				:class="dashboardClass(dashboard)"
				v-for="dashboard in displayedDashboards"
				v-heartbeat="heartbeat(dashboard)"
			>
				<h2 class="dashboard-component-title">{{ dashboard.title || "Missing title" }}</h2>
				<div v-if="!viewport.dashboards.includes(dashboard)" class="dashboard-component-loading">...</div>
				<component
					v-else-if="viewComponent(dashboard)"
					:is="viewComponent(dashboard)"
					:endpoint="endpoint"
					:options="dashboard"
					:timeRange="timeRange"
					:configuration="options.configuration"
					@update:configuration="options = { ...options, configuration: $event }"
					@fetch="onFetch(dashboard, $event)"
					@timestamp="onTimestamp($event)"
					class="dashboard-component-graph"
				>
				</component>
				<div v-else class="dashboard-component-unsupported">Unsupported graph type "{{ dashboard.type }}".</div>
			</div>
		</div>
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import ViewGraph from "#bzd/apps/artifacts/plugins/nodes/frontend/view_graph.vue";
	import ViewLog from "#bzd/apps/artifacts/plugins/nodes/frontend/view_log.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import Utils from "#bzd/apps/artifacts/common/utils.js";
	import { timestampUs } from "#bzd/nodejs/utils/timestamp.js";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.js";
	import DirectiveHeartbeat from "#bzd/nodejs/vue/directives/heartbeat.js";
	import { dateToDefaultString } from "#bzd/nodejs/utils/to_string.js";
	import { arrayFindCommonPrefix } from "#bzd/nodejs/utils/array.js";
	import LocalStorage from "#bzd/nodejs/core/localstorage.js";
	import Lock from "#bzd/nodejs/core/lock.js";

	const optionsStorageKey = "bzd-artifacts-plugin-nodes-view-dashboard-options";

	export default {
		mixins: [Base, Component],
		components: {
			Form,
		},
		directives: {
			loading: DirectiveLoading,
			heartbeat: DirectiveHeartbeat,
		},
		data: function () {
			return {
				dashboards: [],
				viewport: {
					dashboards: [],
					width: 0,
				},
				// Cached information for the getTimestamp function.
				getTimestampCache: null,
				timeRange: [null, null],
				options: {
					interval: "Last 15 minutes",
					tags: null,
					configuration: {},
				},
				// Per-dashboard heartbeat state: { counter, period }, used by the heartbeat directive.
				heartbeats: new Map(),
				periodUs: null,
				lock: new Lock(),
				viewportUpdatedTimeout: null,
				timestampTimer: null,
			};
		},
		watch: {
			async triggerConditionsChange() {
				await this.triggerFetch();
			},
			options: {
				deep: true,
				handler() {
					LocalStorage.setSerializable(optionsStorageKey, this.options);
				},
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
			this.timestampTimer = setInterval(async () => {
				this.loading = true;
				try {
					const timestampNewest = await this.getTimestamp();
					if (timestampNewest !== null) {
						this.timeRange = [timestampNewest - this.periodUs, timestampNewest];
					}
				} finally {
					this.loading = false;
				}
			}, 1000);
		},
		beforeUnmount() {
			window.removeEventListener("scroll", this.handleScroll);
			clearTimeout(this.viewportUpdatedTimeout);
			clearInterval(this.timestampTimer);
		},
		computed: {
			// Conditions that can trigger a new data fetch.
			triggerConditionsChange() {
				return [this.dashboards, this.options.interval, this.viewport];
			},
			intervalOptions() {
				return {
					"Last 5 minutes": async () => {
						await this.useLastPeriod(5 * 60 * 1000000);
					},
					"Last 15 minutes": async () => {
						await this.useLastPeriod(15 * 60 * 1000000);
					},
					"Last 30 minutes": async () => {
						await this.useLastPeriod(30 * 60 * 1000000);
					},
					"Last 1 hour": async () => {
						await this.useLastPeriod(1 * 60 * 60 * 1000000);
					},
					"Last 3 hours": async () => {
						await this.useLastPeriod(3 * 60 * 60 * 1000000);
					},
					"Last 6 hours": async () => {
						await this.useLastPeriod(6 * 60 * 60 * 1000000);
					},
					"Last 12 hours": async () => {
						await this.useLastPeriod(12 * 60 * 60 * 1000000);
					},
					"Last 24 hours": async () => {
						await this.useLastPeriod(24 * 60 * 60 * 1000000);
					},
					"Last 2 days": async () => {
						await this.useLastPeriod(2 * 24 * 60 * 60 * 1000000);
					},
					"Last 7 days": async () => {
						await this.useLastPeriod(7 * 24 * 60 * 60 * 1000000);
					},
					"Last 30 days": async () => {
						await this.useLastPeriod(30 * 24 * 60 * 60 * 1000000);
					},
					"Last 6 months": async () => {
						await this.useLastPeriod(6 * 30 * 24 * 60 * 60 * 1000000);
					},
					"Last 1 year": async () => {
						await this.useLastPeriod(365 * 24 * 60 * 60 * 1000000);
					},
					"Last 2 years": async () => {
						await this.useLastPeriod(2 * 365 * 24 * 60 * 60 * 1000000);
					},
					"Last 5 years": async () => {
						await this.useLastPeriod(5 * 365 * 24 * 60 * 60 * 1000000);
					},
				};
			},
			availableTags() {
				return [...new Set(this.dashboards.map((dashboard) => dashboard.tags || []).flat())].sort();
			},
			selectedTag() {
				const tag = this.options.tags;
				return tag && this.availableTags.includes(tag) ? tag : null;
			},
			displayedDashboards() {
				const tag = this.selectedTag;
				return tag ? this.dashboards.filter((dashboard) => (dashboard.tags || []).includes(tag)) : this.dashboards;
			},
			dashboardEndpoint() {
				const [volume, ...key] = this.pathList;
				return "/x/" + encodeURIComponent(volume) + "/@dashboards" + Utils.keyToPath(key);
			},
			endpoint() {
				const [volume, uid, ..._] = this.pathList;
				return "/x/" + encodeURIComponent(volume) + "/" + encodeURIComponent(uid);
			},
			timeRangeString() {
				const [timestampOldest, timestampNewest] = this.timeRange;
				if (timestampOldest === null && timestampNewest === null) {
					return "...";
				}
				const result = [
					timestampOldest ? dateToDefaultString(timestampOldest / 1000) : "?",
					timestampNewest ? dateToDefaultString(timestampNewest / 1000) : "?",
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
						value: this.displayedDashboards.length > 0 ? this.timeRangeString : "No dashboards",
						align: "center",
					},
				];
			},
		},
		methods: {
			/// Anchor the timestamp extrapolation on the newest timestamp fetched by a dashboard.
			onTimestamp(timestamp) {
				if (timestamp === null || timestamp === undefined) {
					return;
				}
				if (this.getTimestampCache === null || timestamp > this.getTimestampCache.server) {
					this.getTimestampCache = {
						client: timestampUs(),
						server: timestamp,
					};
				}
			},
			/// Record a server fetch of a dashboard and its expected period.
			onFetch(dashboard, period) {
				const previous = this.heartbeats.get(dashboard) ?? { counter: 0 };
				this.heartbeats.set(dashboard, {
					counter: previous.counter + 1,
					period: period ?? previous.period ?? 3000,
				});
			},
			/// Get the heartbeat state of a dashboard, used as the directive value.
			heartbeat(dashboard) {
				return this.heartbeats.get(dashboard) ?? { counter: 0, period: 3000 };
			},
			async selectTag(tag) {
				this.options = Object.assign({}, this.options, { tags: tag || null });
				await this.$nextTick();
				this.viewportUpdated();
			},
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
						viewport.dashboards.push(this.displayedDashboards[index]);
					});
				// Update only if needed.
				if (
					viewport.width != this.viewport.width ||
					!viewport.dashboards.every((d, index) => d === this.viewport.dashboards[index])
				) {
					this.viewport = viewport;
				}
			},
			/// Get the newest timestamp of the data sets, extrapolated to the current time.
			async getTimestamp() {
				if (this.getTimestampCache === null) {
					const timestampBefore = timestampUs();
					const timestampNewest = await this.fetchTimestamp();
					const timestampAfter = timestampUs();

					this.getTimestampCache = {
						client: (timestampAfter + timestampBefore) / 2,
						server: timestampNewest ?? null,
					};
				}

				if (this.getTimestampCache.server === null) {
					return null;
				}
				const elapsedTime = timestampUs() - this.getTimestampCache.client;
				return this.getTimestampCache.server + elapsedTime;
			},
			async useLastPeriod(periodUs) {
				this.periodUs = periodUs;
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
			dashboardClass(dashboard) {
				return {
					"dashboard-component": true,
					["dashboard-component-" + (dashboard.size || "medium")]: true,
				};
			},
			/// Resolve the component responsible for rendering a given dashboard.
			viewComponent(dashboard) {
				switch (dashboard.type) {
					case "linear":
						return ViewGraph;
					case "bar":
						return ViewGraph;
					case "log":
						return ViewLog;
				}
				return null;
			},
			async fetchDashboards() {
				await this.handleSubmit(
					async () => {
						const result = await this.requestBackend(this.dashboardEndpoint, {
							method: "get",
							expect: "json",
						});

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
			/// Fetch the newest timestamp available across all dashboard inputs.
			async fetchTimestamp() {
				return await this.handleSubmit(
					async () => {
						// Make requests as chunks.
						const includes = this.inputsKeysFromDashboards(this.dashboards);
						const promises = [];
						while (includes.length) {
							const chunk = includes.splice(0, 100);
							const query = {
								include: chunk.join(","),
								metadata: 1,
								count: 1,
							};
							promises.push(
								this.requestBackend(this.endpoint, {
									method: "get",
									query: query,
									expect: "json",
								}),
							);
						}
						const results = await Promise.all(promises);

						// Retrieve the newest timestamp from the data itself.
						let timestampNewest = null;
						for (const result of results) {
							for (const [_key, values] of result.data) {
								for (const [timestamp] of values) {
									timestampNewest = timestampNewest === null ? timestamp : Math.max(timestampNewest, timestamp);
								}
							}
						}
						return timestampNewest;
					},
					{ updateLoading: false },
				);
			},
		},
	};
</script>

<style lang="scss" scoped>
	@use "@/nodejs/styles/default/css/form/config.scss" as *;

	.tags {
		display: flex;
		flex-direction: row;
		flex-wrap: wrap;
		gap: 6px;
		padding: 4px 0;

		.tag {
			@extend %clickable;
			@extend %actioner-reset;

			color: $actionerTextColor;
			background-color: $actionerBgColor;
			border-radius: 10px;

			&.active {
				color: $fieldSpecialTextColor;
				background-color: $fieldSpecialBgColor;
			}
		}
	}

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
		.dashboard-component-unsupported,
		.dashboard-component-loading {
			width: 100%;
			height: calc(100% - 30px);
		}

		.dashboard-component-small {
			width: 100%;
			height: 150px;
		}

		.dashboard-component-medium {
			width: 100%;
			height: 300px;
		}

		.dashboard-component-large {
			width: 100%;
			height: 600px;
		}

		@container (width >= 400px) {
			.dashboard-component-small {
				width: 50%;
			}
		}

		@container (width >= 800px) {
			.dashboard-component-small {
				width: 25%;
			}
			.dashboard-component-medium {
				width: 50%;
			}
		}
	}
</style>
