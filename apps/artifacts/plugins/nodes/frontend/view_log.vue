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
			wrap: { mandatory: true, type: Boolean },
		},
		data: function () {
			return {
				lines: [],
				pageSize: 20,
				maxLines: 1000,
				continuation: null,
				pinnedBottom: true,
				fetchingOlder: false,
				fetchingNewer: false,
				liveTimer: null,
			};
		},
		computed: {
			formDescription() {
				return [{ type: "Checkbox", name: "wrap", text: "Wrap" }];
			},
			wrapModel: {
				get() {
					return { wrap: this.wrap };
				},
				set(value) {
					this.$emit("update:wrap", value.wrap);
				},
			},
		},
		async mounted() {
			await this.handleSubmit(async () => {
				await this.fetchInitial();
			});
			this.liveTimer = setInterval(() => {
				this.fetchLive();
			}, livePeriodMs);
		},
		beforeUnmount() {
			clearInterval(this.liveTimer);
		},
		watch: {
			// Reload the logs when a new time window is selected.
			timeRange(previous) {
				const oldSpan = this.span(previous);
				const newSpan = this.span(this.timeRange);
				if (newSpan !== null && oldSpan !== newSpan) {
					this.lines = [];
					this.continuation = null;
					this.fetchInitial();
				}
			},
		},
		emits: ["update:wrap", "fetch"],
		methods: {
			async handleScroll() {
				const container = this.$refs.scrollContainer;
				const distanceToBottom = container.scrollHeight - container.scrollTop - container.clientHeight;
				this.pinnedBottom = distanceToBottom < 100;
				if (container.scrollTop < 100) {
					await this.fetchOlder();
				}
			},
			/// Load the newest page of logs, then fill the viewport with older data until
			/// it overflows or the history is exhausted.
			async fetchInitial() {
				const container = this.$refs.scrollContainer;
				const result = await this.fetchData({ after: this.windowStart() });
				this.lines = this.applyWindow(this.collectEntries(result));
				this.continuation = result.continuation ?? null;
				await this.$nextTick();

				while (
					container.isConnected &&
					this.continuation !== null &&
					container.scrollHeight <= container.clientHeight + 1
				) {
					await this.fetchOlder();
				}

				// Show the newest entries at the bottom when the content overflows.
				if (container.isConnected && container.scrollHeight > container.clientHeight + 1) {
					container.scrollTop = container.scrollHeight;
				}
			},
			async fetchOlder() {
				if (this.fetchingOlder || this.continuation === null) {
					return;
				}
				this.fetchingOlder = true;
				try {
					const result = await this.fetchData({ continuation: this.continuation });
					const entries = this.collectEntries(result);
					this.continuation = result.continuation ?? null;
					if (entries.length === 0) {
						return;
					}
					// Keep only the entries within the time window.
					const inWindow = this.applyWindow(entries);
					if (inWindow.length === 0) {
						// Reached the beginning of the window, stop paging further.
						this.continuation = null;
						return;
					}
					// If the page crosses the window boundary, the next page is entirely outside.
					if (inWindow.length !== entries.length) {
						this.continuation = null;
					}
					await this.addLines(inWindow);
				} finally {
					this.fetchingOlder = false;
				}
			},
			async fetchLive() {
				if (this.fetchingNewer || !this.pinnedBottom) {
					return;
				}
				this.fetchingNewer = true;
				try {
					const newest = this.lines.at(-1)?.[0];
					if (newest === undefined) {
						return;
					}
					const result = await this.fetchData({ after: newest });
					const entries = this.collectEntries(result);
					if (entries.length === 0) {
						return;
					}
					await this.addLines(entries, { keepNewest: true });
				} finally {
					this.fetchingNewer = false;
				}
			},
			/// Merge new entries into the list, trimming to the time window and the maximum
			/// number of lines, while preserving the current scroll position.
			///
			/// \param keepNewest If true, keep the newest lines, otherwise keep the oldest ones.
			async addLines(entries, { keepNewest = false } = {}) {
				const container = this.$refs.scrollContainer;
				if (!container) {
					return;
				}
				const scrollHeightBefore = container.scrollHeight;
				this.lines = this.applyWindow(this.merge(this.lines, entries));
				if (this.lines.length > this.maxLines) {
					if (keepNewest) {
						this.lines.splice(0, this.lines.length - this.maxLines);
					} else {
						this.lines.length = this.maxLines;
					}
				}
				await this.$nextTick();
				container.scrollTop += container.scrollHeight - scrollHeightBefore;
			},
			collectEntries(result) {
				const entries = [];
				for (const [_, values] of result.data) {
					for (const [timestamp, value] of values) {
						entries.push([timestamp, value]);
					}
				}
				return entries.sort((a, b) => a[0] - b[0]);
			},
			merge(entries, newEntries) {
				return [...entries, ...newEntries].sort((a, b) => a[0] - b[0]);
			},
			span(timeRange) {
				return timeRange[0] === null || timeRange[1] === null ? null : timeRange[1] - timeRange[0];
			},
			windowStart() {
				return this.timeRange[0] ?? null;
			},
			applyWindow(entries) {
				const start = this.windowStart();
				return start === null ? entries : entries.filter(([timestamp]) => timestamp >= start);
			},
			async fetchData({ continuation = null, after = null } = {}) {
				this.$emit("fetch", livePeriodMs);
				const query = Object.fromEntries(
					Object.entries({
						include: Object.keys(this.options.inputs).join(","),
						metadata: 1,
						count: this.pageSize,
						continuation: continuation,
						after: after,
					}).filter(([_, value]) => value !== null),
				);
				return await this.requestBackend(this.endpoint, {
					method: "get",
					query: query,
					expect: "json",
				});
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
