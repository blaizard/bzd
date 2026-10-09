<template>
	<div class="view-log">
		<div class="view-log-toolbar">
			<Form :description="formDescription" v-model="wrapModel"></Form>
		</div>
		<div
			class="view-log-lines"
			:class="{ 'view-log-nowrap': !wrap }"
			ref="scrollContainer"
			@scroll="handleScroll"
			v-loading="loading"
		>
			<table>
				<thead>
					<tr>
						<th>Timestamp</th>
						<th>Level</th>
						<th>Topics</th>
						<th>Source</th>
						<th>Message</th>
					</tr>
				</thead>
				<tbody>
					<tr v-if="lines.length === 0">
						<td colspan="5" class="view-log-empty">No logs</td>
					</tr>
					<tr v-for="(line, index) in lines" :key="index" :class="lineClass(line[1])">
						<td v-if="isLegacy(line[1])" colspan="5" class="view-log-message">{{ line[1] }}</td>
						<template v-else>
							<td class="view-log-timestamp">{{ formatTimestamp(line[0]) }}</td>
							<td class="view-log-level">{{ line[1].level || "-" }}</td>
							<td class="view-log-topics">{{ line[1].topic || "-" }}</td>
							<td class="view-log-source">{{ line[1].source || "-" }}</td>
							<td class="view-log-message">{{ line[1].message }}</td>
						</template>
					</tr>
				</tbody>
			</table>
		</div>
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.js";
	import { dateToDefaultString } from "#bzd/nodejs/utils/to_string.js";

	/// The number of entries fetched per request.
	const pageSize = 100;
	/// The maximum number of entries kept in memory.
	const maxLines = 1000;
	/// The expected period between live fetches, in milliseconds.
	const livePeriodMs = 3000;

	export default {
		mixins: [Base, Component],
		components: {
			Form,
		},
		directives: {
			loading: DirectiveLoading,
		},
		props: {
			options: { mandatory: true, type: Object },
			endpoint: { mandatory: true, type: String },
			timeRange: { mandatory: true, type: Array },
			configuration: { type: Object, default: () => ({}) },
		},
		data: function () {
			return {
				lines: [],
				pinnedBottom: true,
				fetching: false,
				liveTimer: null,
			};
		},
		computed: {
			wrap() {
				return this.configuration.wrap ?? false;
			},
			formDescription() {
				return [{ type: "Checkbox", name: "wrap", text: "Wrap" }];
			},
			wrapModel: {
				get() {
					return { wrap: this.wrap };
				},
				set(value) {
					this.$emit("update:configuration", { ...this.configuration, wrap: value.wrap });
				},
			},
			windowStart() {
				return this.timeRange[0] ?? null;
			},
		},
		async mounted() {
			await this.handleSubmit(() => this.loadInitial());
			this.liveTimer = setInterval(() => {
				this.fetchAdjacent("append");
			}, livePeriodMs);
		},
		beforeUnmount() {
			clearInterval(this.liveTimer);
		},
		emits: ["update:configuration", "fetch", "timestamp"],
		methods: {
			handleScroll() {
				const container = this.$refs.scrollContainer;
				const distanceToBottom = container.scrollHeight - container.scrollTop - container.clientHeight;
				this.pinnedBottom = distanceToBottom < 100;
				if (container.scrollTop < 100) {
					this.fetchAdjacent("prepend");
				}
			},
			/// Load the latest page and stick to the bottom.
			async loadInitial() {
				this.lines = await this.fetchEntries();
				this.emitTimestamp();
				this.pinnedBottom = true;
				await this.$nextTick();
				const container = this.$refs.scrollContainer;
				if (container?.isConnected) {
					container.scrollTop = container.scrollHeight;
				}
			},
			/// Fetch the page just before the oldest line ("prepend") or just after the
			/// newest line ("append") and merge it in.
			async fetchAdjacent(direction) {
				if (this.fetching || (direction === "append" && !this.pinnedBottom)) {
					return;
				}
				const reference = direction === "append" ? this.lines.at(-1)?.[0] : this.lines[0]?.[0];
				if (reference === undefined) {
					return;
				}
				this.fetching = true;
				try {
					let entries =
						direction === "append"
							? await this.fetchEntries({ after: reference })
							: await this.fetchEntries({ before: reference });
					if (direction === "prepend") {
						// Discard entries before the time window, so older paging stops at its boundary.
						entries =
							this.windowStart === null ? entries : entries.filter(([timestamp]) => timestamp >= this.windowStart);
					}
					await this.addEntries(entries, { prepend: direction === "prepend" });
				} finally {
					this.fetching = false;
				}
			},
			/// Merge new entries into the list, trimming to the maximum number of lines and
			/// preserving the current scroll position.
			async addEntries(entries, { prepend = false } = {}) {
				if (!entries.length) {
					return;
				}
				const container = this.$refs.scrollContainer;
				const scrollHeightBefore = container?.scrollHeight ?? 0;
				let lines = [...this.lines, ...entries].sort((a, b) => a[0] - b[0]);
				if (lines.length > maxLines) {
					lines = prepend ? lines.slice(0, maxLines) : lines.slice(-maxLines);
				}
				this.lines = lines;
				this.emitTimestamp();
				await this.$nextTick();
				if (container) {
					if (prepend) {
						container.scrollTop += container.scrollHeight - scrollHeightBefore;
					} else {
						container.scrollTop = container.scrollHeight;
					}
				}
			},
			emitTimestamp() {
				const timestampNewest = this.lines.at(-1)?.[0] ?? null;
				if (timestampNewest !== null) {
					this.$emit("timestamp", timestampNewest);
				}
			},
			/// Fetch a page of logs.
			async fetchEntries({ before = null, after = null } = {}) {
				this.$emit("fetch", livePeriodMs);
				const query = Object.fromEntries(
					Object.entries({
						include: Object.keys(this.options.inputs).join(","),
						metadata: 1,
						count: pageSize,
						before: before,
						after: after,
					}).filter(([_, value]) => value !== null),
				);
				const result = await this.requestBackend(this.endpoint, {
					method: "get",
					query: query,
					expect: "json",
				});
				const entries = [];
				for (const [, values] of result.data) {
					for (const [timestamp, value] of values) {
						entries.push([timestamp, value]);
					}
				}
				return entries.sort((a, b) => a[0] - b[0]);
			},
			formatTimestamp(timestampUs) {
				return dateToDefaultString(timestampUs / 1000);
			},
			isLegacy(value) {
				return typeof value === "string";
			},
			lineClass(value) {
				const level = typeof value === "string" ? /^\[(\w+)\]/.exec(value)?.[1] : value.level;
				switch (level) {
					case "error":
						return "view-log-line-error";
					case "warning":
						return "view-log-line-warning";
					case "debug":
					case "trace":
						return "view-log-line-debug";
					default:
						return null;
				}
			},
		},
	};
</script>

<style lang="scss" scoped>
	.view-log {
		height: 100%;
		display: flex;
		flex-direction: column;

		.view-log-toolbar {
			display: flex;
			align-items: center;
			padding: 4px 10px;
		}

		.view-log-lines {
			flex: 1;
			min-height: 0;
			overflow-y: auto;
			overflow-x: auto;
			background-color: #222;
			color: #ddd;
			font-size: 13px;

			table {
				width: 100%;
				border-collapse: collapse;
				font-family: monospace;
			}

			tr:hover {
				background-color: #333;
			}

			th,
			td {
				padding: 4px 10px;
				text-align: left;
				vertical-align: top;
				line-height: 1.4;
				border: none;
			}

			th {
				position: sticky;
				top: 0;
				z-index: 1;
				background-color: #2c2c2c;
				border-bottom: 1px solid #444;
			}

			.view-log-timestamp,
			.view-log-level,
			.view-log-topics,
			.view-log-source {
				white-space: nowrap;
			}

			.view-log-message {
				white-space: pre-wrap;
				word-break: break-all;
			}

			&.view-log-nowrap {
				table {
					width: max-content;
					min-width: 100%;
				}

				.view-log-message {
					white-space: pre;
					word-break: normal;
				}
			}

			.view-log-empty {
				padding: 10px;
				color: #888;
				text-align: center;
			}

			.view-log-line-error {
				color: #ef2929;
				font-weight: 600;
			}

			.view-log-line-warning {
				color: #fce94f;
			}

			.view-log-line-debug {
				color: #888;
			}
		}
	}
</style>
