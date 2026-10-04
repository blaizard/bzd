<template>
	<div class="view-log">
		<div class="view-log-lines" ref="scrollContainer" @scroll="handleScroll" v-loading="loading">
			<div v-if="lines.length === 0" class="view-log-empty">No logs</div>
			<div v-for="(line, index) in lines" :key="index" :class="['view-log-line', lineClass(line[1])]">
				{{ formatLine(line[1]) }}
			</div>
		</div>
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.js";

	export default {
		mixins: [Base, Component],
		directives: {
			loading: DirectiveLoading,
		},
		props: {
			options: { mandatory: true, type: Object },
			endpoint: { mandatory: true, type: String },
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
		async mounted() {
			await this.handleSubmit(async () => {
				await this.fetchInitial();
			});
			this.liveTimer = setInterval(() => {
				this.fetchLive();
			}, 3000);
		},
		beforeUnmount() {
			clearInterval(this.liveTimer);
		},
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
				const result = await this.fetchData({});
				this.lines = this.collectEntries(result);
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
					const container = this.$refs.scrollContainer;
					if (!container) {
						return;
					}
					const scrollHeightBefore = container.scrollHeight;
					const result = await this.fetchData({ continuation: this.continuation });
					const entries = this.collectEntries(result);
					this.continuation = result.continuation ?? null;
					if (entries.length === 0) {
						return;
					}
					this.lines = this.merge(this.lines, entries);
					// Keep the oldest entries, the newest ones are out of view.
					if (this.lines.length > this.maxLines) {
						this.lines.length = this.maxLines;
					}
					await this.$nextTick();
					container.scrollTop += container.scrollHeight - scrollHeightBefore;
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
					const container = this.$refs.scrollContainer;
					if (newest === undefined || !container) {
						return;
					}
					const scrollHeightBefore = container.scrollHeight;
					const result = await this.fetchData({ after: newest });
					const entries = this.collectEntries(result);
					if (entries.length === 0) {
						return;
					}
					this.lines = this.merge(this.lines, entries);
					// Keep the newest entries, the oldest ones are out of view.
					if (this.lines.length > this.maxLines) {
						this.lines.splice(0, this.lines.length - this.maxLines);
					}
					await this.$nextTick();
					container.scrollTop += container.scrollHeight - scrollHeightBefore;
				} finally {
					this.fetchingNewer = false;
				}
			},
			collectEntries(result) {
				const entries = [];
				for (const [_, values] of result.data) {
					for (const [timestamp, value] of values) {
						entries.push([timestamp, value]);
					}
				}
				return this.merge([], entries);
			},
			merge(entries, newEntries) {
				return [...entries, ...newEntries].sort((a, b) => a[0] - b[0]);
			},
			async fetchData({ continuation = null, after = null } = {}) {
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
			/// Reconstruct the display string of a log entry.
			///
			/// The value is either a legacy string or a dictionary of log fields.
			formatLine(value) {
				if (typeof value === "string") {
					return value;
				}
				const parts = [];
				if (value.level) {
					parts.push(`[${value.level}]`);
				}
				if (value.topic) {
					parts.push(`[${value.topic}]`);
				}
				if (value.source) {
					parts.push(`[${value.source}]`);
				}
				parts.push(value.message ?? "");
				return parts.join(" ");
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

		.view-log-lines {
			height: 100%;
			overflow-y: auto;
			background-color: #222;
			color: #ddd;
			font-family: monospace;
			font-size: 13px;
			padding: 10px 0;

			.view-log-line {
				padding: 0 10px;
				line-height: 1.4;
				white-space: pre-wrap;
				word-break: break-all;
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

			.view-log-empty {
				padding: 10px;
				color: #888;
			}
		}
	}
</style>
