<template>
	<div class="bzd-statistics">
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
		<div v-else class="bzd-statistics-tree" v-if="items.length">
			<template v-for="item in items" :key="item.key">
				<!-- Branch node -->
				<div
					v-if="item.type === 'branch'"
					:style="{ paddingLeft: item.depth * 20 + 'px' }"
					class="bzd-statistics-branch"
					@click="toggleNode(item.key)"
				>
					<span class="bzd-statistics-branch-arrow">{{ item.expanded ? "▾" : "▸" }}</span>
					<span class="bzd-statistics-branch-name">{{ item.name }}</span>
				</div>
				<!-- Leaf node (metric card) -->
				<div v-else class="bzd-statistics-card-wrapper">
					<div class="bzd-statistics-card" :class="'bzd-statistics-type-' + item.type">
						<div class="bzd-statistics-title">{{ item.name }}</div>
						<div class="bzd-statistics-badge">{{ item.type }}</div>
						<div class="bzd-statistics-value">{{ formatMetric(item.metric, item.path) }}</div>
						<div v-if="getDetails(item.metric, item.path).length" class="bzd-statistics-details">
							<div
								v-for="detail in getDetails(item.metric, item.path)"
								:key="detail.label"
								class="bzd-statistics-detail"
							>
								<span class="bzd-statistics-detail-label">{{ detail.label }}:</span>
								<span class="bzd-statistics-detail-value">{{ detail.value }}</span>
							</div>
						</div>
					</div>
				</div>
			</template>
		</div>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Checkbox from "#bzd/nodejs/vue/components/form/element/checkbox.vue";
	import { bytesToString, timeMsToString, dateToDefaultString } from "#bzd/nodejs/utils/to_string.mjs";

	export default {
		mixins: [Component],
		components: {
			Checkbox,
		},
		data: function () {
			return {
				autoRefresh: true,
				refreshIntervalMs: 5000,
				timestamp: null,
				statistics: {},
				collapsed: [],
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
				// Flatten raw statistics into a sorted tree, then flatten with depth for rendering
				const metrics = [];
				for (const [uid, value] of Object.entries(this.statistics)) {
					metrics.push(...this.buildMetrics(value, uid));
				}
				// Build tree from flat metrics by grouping common path segments
				const root = [];
				for (const item of metrics) {
					const segments = item.path;
					let current = root;
					for (let i = 0; i < segments.length; i++) {
						const segment = segments[i];
						const isLeaf = i === segments.length - 1;
						let node = current.find((n) => n.name === segment);
						if (!node) {
							node = { name: segment, key: segments.slice(0, i + 1).join(".") };
							if (isLeaf) {
								Object.assign(node, { metric: item.metric, type: item.type, path: item.path });
							} else {
								node.children = [];
							}
							current.push(node);
						}
						current = node.children || [];
					}
				}
				// Sort children at every level alphabetically by name
				const sortNodes = (nodes) => {
					nodes.sort((a, b) => a.name.localeCompare(b.name));
					for (const node of nodes) {
						if (node.children) {
							sortNodes(node.children);
						}
					}
				};
				sortNodes(root);
				// Flatten tree with depth, respecting collapsed state
				const result = [];
				const walk = (nodes, depth) => {
					for (const node of nodes) {
						if (node.children) {
							const isExpanded = !this.collapsed.includes(node.key);
							result.push({ type: "branch", name: node.name, key: node.key, depth: depth, expanded: isExpanded });
							if (isExpanded) {
								walk(node.children, depth + 1);
							}
						} else {
							result.push({
								type: "leaf",
								name: node.name,
								key: node.key,
								depth: depth,
								metric: node.metric,
								type: node.type,
								path: node.path,
							});
						}
					}
				};
				walk(root, 0);
				return result;
			},
		},
		methods: {
			toggleNode(key) {
				const index = this.collapsed.indexOf(key);
				if (index !== -1) {
					this.collapsed.splice(index, 1);
				} else {
					this.collapsed.push(key);
				}
			},
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
				const metricKeys = ["value", "sum", "rate", "duration", "rateAvg", "count", "max", "min", "avg"];
				const keys = Object.keys(value);
				return (
					keys.length > 0 &&
					keys.some((key) => metricKeys.includes(key)) &&
					Object.values(value).every((v) => v === null || ["boolean", "number", "string"].includes(typeof v))
				);
			},
			detectMetricType(metric) {
				if ("rate" in metric) {
					return "rate";
				}
				if ("duration" in metric) {
					return "time";
				}
				if ("value" in metric || "sum" in metric) {
					return "value";
				}
				return "unknown";
			},
			detectUnit(path) {
				const name = path[path.length - 1].toLowerCase();
				if (name.includes("size") || name.includes("bytes")) {
					return "bytes";
				}
				if (name.includes("time") || name.includes("duration") || name.includes("latency")) {
					return "time";
				}
				return null;
			},
			formatNumber(value) {
				return parseFloat(value ?? 0).toFixed(2);
			},
			formatValue(key, value, { type, unit } = {}) {
				if (key === "rate" || key === "rateAvg") {
					return this.formatNumber(value) + "/s";
				}
				if (key === "duration") {
					return timeMsToString(value);
				}
				if (key === "count") {
					return this.formatNumber(value);
				}
				if (unit && (key === "value" || key === "sum" || key === "max" || key === "min" || key === "avg")) {
					if (unit === "bytes") {
						return bytesToString(value);
					}
					if (unit === "time") {
						return timeMsToString(value);
					}
				}
				return this.formatNumber(value);
			},
			formatMetric(metric, path) {
				const type = this.detectMetricType(metric);
				if (type === "rate") {
					return this.formatValue("rate", metric.rate, { type });
				}
				if (type === "time") {
					return this.formatValue("duration", metric.duration, { type });
				}
				if (type === "value") {
					const primaryKey = "sum" in metric ? "sum" : "value";
					const unit = this.detectUnit(path);
					return this.formatValue(primaryKey, metric[primaryKey], { type, unit });
				}
				// Unknown type: show a summary of all fields
				return Object.entries(metric)
					.map(([key, value]) => key + "=" + this.formatValue(key, value))
					.join(", ");
			},
			getDetails(metric, path) {
				const details = [];
				const type = this.detectMetricType(metric);
				const unit = type === "value" ? this.detectUnit(path) : null;

				// Keys already shown as the primary value in the card header
				const primaryKeys =
					{
						rate: ["rate"],
						time: ["duration"],
						value: ["value", "sum"],
						unknown: [],
					}[type] || [];

				// Display priority: max, min, avg, count first, then everything else alphabetically
				const priority = ["max", "min", "avg", "count"];
				const sortedKeys = Object.keys(metric).sort((a, b) => {
					const ia = priority.indexOf(a);
					const ib = priority.indexOf(b);
					if (ia !== -1 && ib !== -1) return ia - ib;
					if (ia !== -1) return -1;
					if (ib !== -1) return 1;
					return a.localeCompare(b);
				});

				for (const key of sortedKeys) {
					if (primaryKeys.includes(key)) {
						continue;
					}
					details.push({
						label: key,
						value: this.formatValue(key, metric[key], { type, unit }),
					});
				}
				return details;
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

		.bzd-statistics-loading {
			color: config.$bzdGraphColorGray;
		}

		.bzd-statistics-branch {
			cursor: pointer;
			user-select: none;
			padding: 4px 0;
			margin: 4px 0;
			display: flex;
			align-items: center;
			gap: 4px;

			.bzd-statistics-branch-arrow {
				font-size: 0.85em;
				color: config.$bzdGraphColorGray;
				width: 16px;
				flex-shrink: 0;
			}

			.bzd-statistics-branch-name {
				font-weight: 600;
				color: config.$bzdGraphColorBlack;
			}
		}

		.bzd-statistics-card-wrapper {
			display: inline-block;
			vertical-align: top;
			margin: 4px;
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

			&.bzd-statistics-type-unknown {
				border-left-color: config.$bzdGraphColorPink;
			}
		}
	}
</style>
