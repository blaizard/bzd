<template>
	<div class="bzd-statistics">
		<div class="bzd-statistics-search">
			<Input :description="{ placeholder: 'Filter...' }" @directInput="searchQuery = $event" />
		</div>
		<div class="bzd-statistics-toolbar">
			<Checkbox
				:model-value="autoRefresh"
				:description="{ text: 'Auto-refresh' }"
				@update:model-value="autoRefresh = $event"
			/>
			<span v-if="timestamp" class="bzd-statistics-timestamp">
				{{ formatTimestamp(timestamp) }}
			</span>
			<button @click="handleFetch" :disabled="loading">Refresh</button>
		</div>
		<div v-if="loading && !timestamp" class="bzd-statistics-loading">Loading...</div>
		<div v-else-if="items.length" class="bzd-statistics-list">
			<template v-for="group in filteredItems" :key="group.key">
				<div class="bzd-statistics-group">
					<div class="bzd-statistics-group-title">{{ group.name }}</div>
					<div class="bzd-statistics-group-cards">
						<div
							v-for="card in group.metrics"
							:key="card.key"
							class="bzd-statistics-card"
							:class="'bzd-statistics-type-' + card.type"
						>
							<div class="bzd-statistics-title">{{ card.name }}</div>
							<div class="bzd-statistics-badge">{{ card.type }}</div>
							<div class="bzd-statistics-value">{{ formatMetric(card.metric) }}</div>
							<div v-if="getDetails(card.metric).length" class="bzd-statistics-details">
								<div v-for="detail in getDetails(card.metric)" :key="detail.label" class="bzd-statistics-detail">
									<span class="bzd-statistics-detail-label">{{ detail.label }}:</span>
									<span class="bzd-statistics-detail-value">{{ detail.value }}</span>
								</div>
							</div>
						</div>
					</div>
				</div>
			</template>
		</div>
		<div v-else class="bzd-statistics-empty">No statistics available.</div>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Checkbox from "#bzd/nodejs/vue/components/form/element/checkbox.vue";
	import Input from "#bzd/nodejs/vue/components/form/element/input.vue";
	import { bytesToString, timeMsToString, dateToDefaultString } from "#bzd/nodejs/utils/to_string.js";

	export default {
		mixins: [Component],
		components: {
			Checkbox,
			Input,
		},
		data: function () {
			return {
				autoRefresh: true,
				refreshIntervalMs: 5000,
				timestamp: null,
				statistics: {},
				searchQuery: "",
			};
		},
		mounted() {
			this.handleFetch();
		},
		beforeUnmount() {
			this.stopTimer();
		},
		watch: {
			autoRefresh(newVal) {
				if (newVal) {
					this.startTimer();
				} else {
					this.stopTimer();
				}
			},
		},
		computed: {
			items() {
				const allMetrics = [];
				for (const [uid, value] of Object.entries(this.statistics)) {
					allMetrics.push(...this.buildMetrics(value, uid));
				}
				const groups = new Map();
				for (const m of allMetrics) {
					const parentPath = [m.uid, ...m.path.slice(0, -1)];
					const joined = parentPath.join(".");
					const groupKey = joined || Symbol.for("bzd.statistics.root");
					let group = groups.get(groupKey);
					if (!group) {
						group = {
							key: groupKey,
							name: typeof groupKey === "symbol" ? "(root)" : joined,
							metrics: [],
						};
						groups.set(groupKey, group);
					}
					group.metrics.push({
						key: [m.uid, ...m.path].join("."),
						name: m.path[m.path.length - 1],
						metric: m.metric,
						type: m.type,
					});
				}
				const result = [...groups.values()];
				result.sort((a, b) => a.name.localeCompare(b.name));
				for (const g of result) {
					g.metrics.sort((a, b) => a.name.localeCompare(b.name));
				}
				return result;
			},
			filteredItems() {
				const query = this.searchQuery.trim().toLowerCase();
				if (!query) {
					return this.items;
				}
				return this.items
					.map((group) => {
						const groupMatch = group.name.toLowerCase().includes(query);
						if (groupMatch) {
							return group;
						}
						const matchingMetrics = group.metrics.filter((card) => {
							return card.key.toLowerCase().includes(query);
						});
						if (matchingMetrics.length > 0) {
							return { ...group, metrics: matchingMetrics };
						}
						return null;
					})
					.filter((g) => g !== null);
			},
		},
		methods: {
			startTimer() {
				if (!this.autoRefresh || this.refreshTimer || this.loading) {
					return;
				}
				this.refreshTimer = setTimeout(() => {
					this.refreshTimer = null;
					this.handleFetch();
				}, this.refreshIntervalMs);
			},
			stopTimer() {
				clearTimeout(this.refreshTimer);
				this.refreshTimer = null;
			},
			async handleFetch() {
				if (this.loading) {
					return;
				}
				this.stopTimer();
				await this.handleSubmit(async () => {
					const result = await this.$rest.request("get", "/admin/statistics");
					this.statistics = result.data;
					this.timestamp = Date.now();
				});
				if (this.autoRefresh) {
					this.startTimer();
				}
			},
			formatTimestamp(timestamp) {
				return dateToDefaultString(timestamp);
			},
			isMetric(value) {
				if (value === null || typeof value !== "object" || Array.isArray(value)) {
					return false;
				}
				const metricKeys = ["value", "sum", "size", "rate", "duration", "rateAvg", "count", "max", "min", "avg"];
				const keys = Object.keys(value);
				return (
					keys.length > 0 &&
					keys.some((key) => metricKeys.includes(key)) &&
					Object.values(value).every((v) => v === null || ["boolean", "number", "string"].includes(typeof v))
				);
			},
			detectMetricType(metric) {
				if ("rate" in metric) return "rate";
				if ("duration" in metric) return "time";
				if ("size" in metric) return "size";
				if ("sum" in metric) return "sum";
				if ("value" in metric) return "value";
				return "unknown";
			},
			formatNumber(value) {
				return parseFloat(value ?? 0).toFixed(2);
			},
			formatMetric(metric) {
				const type = this.detectMetricType(metric);
				if (type === "rate") return this.formatNumber(metric.rate) + "/s";
				if (type === "time") return timeMsToString(metric.duration);
				if (type === "size") return bytesToString(metric.size);
				if (type === "sum") return this.formatNumber(metric.sum);
				if (type === "value") return this.formatNumber(metric.value);
				// Unknown type: show a summary of all fields
				return Object.entries(metric)
					.map(([key, value]) => key + "=" + this.formatNumber(value))
					.join(", ");
			},
			getDetails(metric) {
				const type = this.detectMetricType(metric);
				const primaryKeys =
					{ rate: ["rate"], time: ["duration"], size: ["size"], sum: ["sum"], value: ["value"] }[type] || [];
				const unit = type === "size" ? "bytes" : type === "time" ? "time" : null;

				return Object.entries(metric)
					.filter(([key]) => !primaryKeys.includes(key))
					.map(([key, value]) => ({
						label: key,
						value:
							key === "count"
								? this.formatNumber(value)
								: key === "rateAvg"
									? this.formatNumber(value) + "/s"
									: unit === "bytes"
										? bytesToString(value)
										: unit === "time"
											? timeMsToString(value)
											: this.formatNumber(value),
					}));
			},
			buildMetrics(obj, uid, path = []) {
				const metrics = [];
				for (const [key, value] of Object.entries(obj)) {
					if (this.isMetric(value)) {
						const metricPath = [...path, key];
						metrics.push({
							uid: uid,
							name: metricPath.join("."),
							path: metricPath,
							metric: value,
							type: this.detectMetricType(value),
						});
					} else if (value !== null && typeof value === "object" && !Array.isArray(value)) {
						const childPath = [...path, key];
						const childKeys = Object.keys(value);
						// Dedup: when a provider is registered with the same UID as its namespace prefix,
						// the tree has a redundant nesting level (e.g., { "plugins": { "plugins": { ... } } }).
						// Skip it when the child object has exactly one key which matches.
						if (childKeys.length === 1 && childKeys[0] === key) {
							metrics.push(...this.buildMetrics(value, uid, path));
						} else {
							metrics.push(...this.buildMetrics(value, uid, childPath));
						}
					}
				}
				return metrics;
			},
		},
	};
</script>

<style lang="scss" scoped>
	@use "@/config.scss" as config;

	.bzd-statistics {
		.bzd-statistics-toolbar {
			display: flex;
			align-items: center;
			gap: 16px;
			margin-bottom: 16px;

			label {
				display: flex;
				align-items: center;
				gap: 6px;
			}

			.bzd-statistics-timestamp {
				color: config.$bzdGraphColorGray;
				font-size: 0.9em;
			}
		}

		.bzd-statistics-search {
			margin-bottom: 16px;
		}

		.bzd-statistics-loading {
			color: config.$bzdGraphColorGray;
		}

		.bzd-statistics-list {
			display: flex;
			flex-direction: column;
			gap: 16px;
		}

		.bzd-statistics-group-title {
			font-weight: bold;
			color: config.$bzdGraphColorBlack;
			margin-bottom: 8px;
			overflow-wrap: anywhere;
		}

		.bzd-statistics-group-cards {
			display: flex;
			flex-wrap: wrap;
			gap: 8px;
		}

		.bzd-statistics-empty {
			color: config.$bzdGraphColorGray;
		}

		.bzd-statistics-card {
			border: 1px solid config.$bzdGraphColorGray;
			border-left-width: 6px;
			border-radius: 4px;
			padding: 12px;
			position: relative;

			.bzd-statistics-title {
				font-weight: bold;
				word-break: break-word;
				padding-right: 70px;
			}

			.bzd-statistics-badge {
				position: absolute;
				top: 8px;
				right: 8px;
				font-size: 0.75em;
				text-transform: uppercase;
				padding: 2px 6px;
				border-radius: 3px;
				background-color: config.$bzdGraphColorGray;
				color: config.$bzdGraphColorWhite;
			}

			.bzd-statistics-value {
				margin-top: 8px;
				font-size: 1.4em;
			}

			.bzd-statistics-details {
				margin-top: 8px;
				padding-top: 8px;
				border-top: 1px solid config.$bzdGraphColorGray;
				display: flex;
				flex-wrap: wrap;
				gap: 4px 12px;
			}

			.bzd-statistics-detail {
				font-size: 0.85em;
				color: config.$bzdGraphColorBlack;

				.bzd-statistics-detail-label {
					color: config.$bzdGraphColorGray;
				}

				.bzd-statistics-detail-value {
					font-weight: bold;
				}
			}

			&.bzd-statistics-type-rate {
				border-left-color: config.$bzdGraphColorBlue;
			}

			&.bzd-statistics-type-time {
				border-left-color: config.$bzdGraphColorViolet;
			}

			&.bzd-statistics-type-value {
				border-left-color: config.$bzdGraphColorGreen;
			}

			&.bzd-statistics-type-size {
				border-left-color: config.$bzdGraphColorGreen;
			}

			&.bzd-statistics-type-sum {
				border-left-color: config.$bzdGraphColorGreen;
			}

			&.bzd-statistics-type-unknown {
				border-left-color: config.$bzdGraphColorPink;
			}
		}
	}
</style>
