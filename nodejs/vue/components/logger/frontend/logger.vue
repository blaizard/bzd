<template>
	<div class="bzd-logger">
		<div class="bzd-logger-form">
			<Form v-model="form" :description="description"></Form>
		</div>
		<div class="bzd-logger-table-wrapper" ref="scrollContainer">
			<table>
				<colgroup>
					<col class="col-time" />
					<col class="col-level" />
					<col class="col-topics" />
					<col class="col-message" />
				</colgroup>
				<thead>
					<tr>
						<th>Time</th>
						<th>Level</th>
						<th>Topics</th>
						<th>Message</th>
					</tr>
				</thead>
				<tbody>
					<tr v-for="log in filteredLogs" :class="['bzd-logger-row', `bzd-logger-row--${log.level}`]">
						<td>{{ log.date }}</td>
						<td>{{ log.level }}</td>
						<td>{{ formatTopics(log.topics) }}</td>
						<td class="message">{{ log.message }}</td>
					</tr>
				</tbody>
				<tr v-if="filteredLogs.length === 0">
					<td colspan="4" class="bzd-logger-empty">No logs available</td>
				</tr>
			</table>
		</div>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";

	export default {
		mixins: [Component],
		components: {
			Form,
		},
		mounted() {
			this.handleFetch();
		},
		data: function () {
			return {
				logs: [],
				form: {
					minLevel: "info",
				},
			};
		},
		computed: {
			description() {
				return [
					{
						type: "Dropdown",
						name: "minLevel",
						caption: "Level",
						list: ["debug", "info", "warning", "error"],
						width: 0.1,
					},
				];
			},
			showOnlyLevels() {
				switch (this.form.minLevel) {
					case "debug":
						return new Set(["trace", "debug", "info", "warning", "error"]);
					case "info":
						return new Set(["info", "warning", "error"]);
					case "warning":
						return new Set(["warning", "error"]);
					case "error":
						return new Set(["error"]);
					default:
						return new Set();
				}
			},
			filteredLogs() {
				return this.logs.filter((log) => {
					return this.showOnlyLevels.has(log.level);
				});
			},
		},
		methods: {
			formatTopics(topics) {
				if (Array.isArray(topics)) {
					return topics.join("::");
				}
				return "-";
			},
			async handleFetch() {
				await this.handleSubmit(async () => {
					const result = await this.$rest.request("get", "/admin/logger");
					const merged = [];
					for (const entries of Object.values(result.logs)) {
						for (const entry of entries) {
							merged.push(entry);
						}
					}
					merged.sort((a, b) => (a.date < b.date ? -1 : a.date > b.date ? 1 : 0));
					this.logs = merged;
					await this.scrollToBottom();
				});
			},
			scrollToBottom() {
				return this.$nextTick(() => {
					const container = this.$refs.scrollContainer;
					if (container) {
						container.scrollTop = container.scrollHeight;
					}
				});
			},
		},
	};
</script>

<style lang="scss" scoped>
	@use "@/config.scss" as config;
	@use "sass:color" as color;

	.bzd-logger {
		display: flex;
		flex-direction: column;
		max-height: 100vh;
		min-height: 0;
	}

	.bzd-logger-table-wrapper {
		flex: 1 1 auto;
		min-height: 0;
		overflow: auto;
		border: 1px solid color.adjust(config.$bzdGraphColorWhite, $lightness: -10%);
	}

	.bzd-logger-form {
		flex-shrink: 0;
	}

	table {
		width: 100%;
		border-collapse: collapse;
		table-layout: fixed;
		font-size: 13px;
	}

	.col-time {
		width: 15%;
	}
	.col-level {
		width: 8%;
	}
	.col-topics {
		width: 20%;
	}
	.col-message {
		width: 57%;
	}

	th,
	td {
		padding: 6px 10px;
		text-align: left;
		vertical-align: top;
	}

	td:nth-child(-n + 3) {
		overflow: hidden;
		text-overflow: ellipsis;
		white-space: nowrap;
	}

	th {
		position: sticky;
		top: 0;
		z-index: 1;
		background-color: #f5f5f5;
		border-bottom: 1px solid color.adjust(config.$bzdGraphColorWhite, $lightness: -15%);
	}

	.bzd-logger-row--trace {
		color: config.$bzdGraphColorBlue;
	}
	.bzd-logger-row--debug {
		color: config.$bzdGraphColorGray;
	}
	/* info: no special color — uses default text color */
	.bzd-logger-row--warning {
		color: config.$bzdGraphColorOrange;
	}
	.bzd-logger-row--error {
		color: config.$bzdGraphColorRed;
		font-weight: 600;
	}

	.message {
		font-family: monospace;
		white-space: pre-wrap;
		word-break: break-all;
	}

	.bzd-logger-empty {
		text-align: center;
		padding: 20px;
		color: config.$bzdGraphColorGray;
	}

	tr:hover {
		background: inherit;
	}
</style>
