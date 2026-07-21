<template>
	<div class="services-view" v-loading="loading && !hasServices">
		<div class="services-toolbar">
			<div class="refresh-indicator">Updated: {{ refreshedAgo }} ago</div>
		</div>
		<div v-for="(data, uid) in services" :key="uid" class="service-card" :class="serviceStatusClass(data.state)">
			<h2>{{ uid }}</h2>
			<div class="service-header">
				<div class="entry">
					<span class="key">Status: </span><code>{{ data.state.status }}</code>
				</div>
				<div class="entry">
					<span class="key">Last Start: </span><code>{{ timestampToDurationString(data.state.timestampStart) }}</code>
				</div>
				<div class="entry">
					<span class="key">Last Stop: </span><code>{{ timestampToDurationString(data.state.timestampStop) }}</code>
				</div>
			</div>
			<table>
				<thead>
					<tr>
						<th>Process</th>
						<th>Status</th>
						<th># Executions</th>
						<th># Errors</th>
						<th>Min</th>
						<th>Max</th>
						<th>Average</th>
						<th>Interval</th>
						<th><!-- actions --></th>
					</tr>
				</thead>
				<tbody>
					<template v-for="(process, name) in data.processes" :key="name">
						<tr :class="['process-row', processStatusClass(process)]">
							<td>{{ uid }}.{{ name }}</td>
							<td>
								<span class="status-text">{{ process.status }}</span>
							</td>
							<td>{{ process.executions }}</td>
							<td :class="['error-count', { 'has-errors': process.errors > 0 }]">{{ process.errors }}</td>
							<td>{{ durationSToString(process.durationMin) }}</td>
							<td>{{ durationSToString(process.durationMax) }}</td>
							<td>{{ durationSToString(process.durationAvg) }}</td>
							<td>{{ process.estimatedInterval ? durationSToString(process.estimatedInterval) : "-" }}</td>
							<td class="actions">
								<button @click="processTrigger(uid, name)" :disabled="loading">Trigger</button>
								<button @click="toggleLogs(uid, name)" :disabled="loading">
									{{ isLogsShownFor(uid, name) ? "Hide Logs" : "Show Logs" }}
								</button>
							</td>
						</tr>
						<tr v-if="lastError(process)" class="process-error-row">
							<td colspan="9">
								<span class="error-label">Last error:</span>
								<span class="error-message">{{ lastError(process) }}</span>
							</td>
						</tr>
					</template>
				</tbody>
			</table>
			<div v-if="logs[uid]" class="logs-panel">
				<div class="logs-header">
					<h2>Logs for {{ uid }}.{{ logs[uid].name }}</h2>
					<button class="logs-close" @click="closeLogs(uid)">Close</button>
				</div>
				<table>
					<thead>
						<tr>
							<th>Start</th>
							<th>Status</th>
							<th>Duration</th>
							<th>Output</th>
						</tr>
					</thead>
					<tbody>
						<tr v-for="log in getSortedLogs(uid)" :key="log.timestampStart" :class="['log-row', logStatusClass(log)]">
							<td>{{ timestampToString(log.timestampStart) }}</td>
							<td>{{ logToStatus(log) }}</td>
							<td>{{ logToDuration(log) }}</td>
							<td class="log-output">{{ logToOutput(log) }}</td>
						</tr>
					</tbody>
				</table>
			</div>
		</div>
		<div v-if="!loading && !hasServices" class="empty-state">No services available.</div>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.js";
	import { timeToString, dateToDefaultString } from "#bzd/nodejs/utils/to_string.js";

	export default {
		mixins: [Component],
		directives: { loading: DirectiveLoading },
		mounted() {
			this.handleFetch();
			this.startPolling();
			this.startTicking();
		},
		beforeUnmount() {
			this.stopPolling();
			this.stopTicking();
		},
		data() {
			return {
				services: {},
				logs: {},
				timestamp: 0,
				pollingTimer: null,
				lastRefreshedAt: Date.now(),
				tick: 0,
				tickTimer: null,
			};
		},
		computed: {
			hasServices() {
				return Object.keys(this.services).length > 0;
			},
			refreshedAgo() {
				// Access tick to trigger re-evaluation on every timer tick
				void this.tick;
				const seconds = (Date.now() - this.lastRefreshedAt) / 1000;
				return timeToString(Math.max(seconds, 0));
			},
		},
		methods: {
			timestampToDurationString(timestamp) {
				if (!timestamp) {
					return "-";
				}
				const durationMs = this.timestamp - timestamp;
				return timeToString(durationMs / 1000);
			},
			durationSToString(durationS) {
				return timeToString(durationS);
			},
			logToStatus(log) {
				if (log.timestampStop === 0) {
					return "running";
				} else if (log.error === null) {
					return "completed";
				} else {
					return "error";
				}
			},
			logToDuration(log) {
				if (!log.timestampStop) {
					return "-";
				}
				return timeToString((log.timestampStop - log.timestampStart) / 1000);
			},
			logToOutput(log) {
				if (log.error !== null) {
					return log.error;
				} else if (log.result) {
					return JSON.stringify(log.result);
				}
				return "-";
			},
			timestampToString(timestamp) {
				return dateToDefaultString(timestamp);
			},
			startPolling() {
				this.pollingTimer = setInterval(async () => {
					try {
						const result = await this.$rest.request("get", "/admin/services");
						this.services = result.services;
						this.timestamp = result.timestamp;
						this.lastRefreshedAt = Date.now();

						// Refresh any open logs panels
						for (const uid of Object.keys(this.logs)) {
							const entry = this.logs[uid];
							if (entry && entry.name) {
								try {
									const logsResult = await this.$rest.request("get", "/admin/service/logs", { uid, name: entry.name });
									this.logs[uid] = { name: entry.name, logs: logsResult.logs };
								} catch {
									// Silently ignore log refresh errors
								}
							}
						}
					} catch {
						// Silently ignore polling errors
					}
				}, 2000);
			},
			stopPolling() {
				if (this.pollingTimer) {
					clearInterval(this.pollingTimer);
					this.pollingTimer = null;
				}
			},
			startTicking() {
				// Update every 200ms so the "Updated: X ago" counter moves smoothly
				this.tickTimer = setInterval(() => {
					this.tick++;
				}, 1000);
			},
			stopTicking() {
				if (this.tickTimer) {
					clearInterval(this.tickTimer);
					this.tickTimer = null;
				}
			},
			async handleFetch() {
				await this.handleSubmit(async () => {
					const result = await this.$rest.request("get", "/admin/services");
					this.services = result.services;
					this.timestamp = result.timestamp;
				});
			},
			async processShowLogs(uid, name) {
				await this.handleSubmit(async () => {
					const result = await this.$rest.request("get", "/admin/service/logs", { uid, name });
					// Store per-card so each service card manages its own logs panel
					this.logs[uid] = { name: name, logs: result.logs };
				});
			},
			async processTrigger(uid, name) {
				this.loading = true;
				try {
					await this.$rest.request("post", "/admin/service/trigger", { uid, name });
					this.$notification.success(`Triggered ${uid}.${name}`);
					await this.handleFetch();
				} catch (e) {
					this.handleError(e);
				} finally {
					this.loading = false;
				}
			},
			isLogsShownFor(uid, name) {
				return this.logs[uid] && this.logs[uid].name === name;
			},
			toggleLogs(uid, name) {
				if (this.isLogsShownFor(uid, name)) {
					// Close: remove the entry entirely
					const newLogs = { ...this.logs };
					delete newLogs[uid];
					this.logs = newLogs;
				} else {
					this.processShowLogs(uid, name);
				}
			},
			closeLogs(uid) {
				const newLogs = { ...this.logs };
				delete newLogs[uid];
				this.logs = newLogs;
			},
			getSortedLogs(uid) {
				const entry = this.logs[uid];
				if (!entry || !entry.logs) return [];
				return [...entry.logs].sort((a, b) => b.timestampStart - a.timestampStart);
			},
			serviceStatusClass(state) {
				switch (state.status) {
					case "running":
						return "status-running";
					case "error":
						return "status-error";
					case "starting":
						return "status-starting";
					case "stopping":
						return "status-stopping";
					default:
						return "status-idle";
				}
			},
			processStatusClass(process) {
				switch (process.status) {
					case "running":
						return "status-running";
					case "error":
						return "status-error";
					case "starting":
						return "status-starting";
					case "stopping":
						return "status-stopping";
					default:
						return "status-idle";
				}
			},
			lastError(process) {
				return process.errorLogs && process.errorLogs.length ? String(process.errorLogs[0].error) : null;
			},
			logStatusClass(log) {
				if (log.timestampStop === 0) return "log-running";
				if (log.error) return "log-error";
				return "log-completed";
			},
		},
	};
</script>

<style lang="scss" scoped>
	@use "@/config.scss" as config;
	@use "sass:color" as color;

	.services-view {
		display: flex;
		flex-direction: column;
		gap: 16px;
	}

	.services-toolbar {
		display: flex;
		justify-content: flex-start;
		align-items: center;
		margin-bottom: 8px;
		min-height: 24px;
	}

	.refresh-indicator {
		font-size: 12px;
		color: color.adjust(config.$bzdGraphColorGray, $lightness: -20%);
		user-select: none;
	}

	.service-card {
		border: 1px solid color.adjust(config.$bzdGraphColorWhite, $lightness: -10%);
		border-left: 4px solid config.$bzdGraphColorGray;
		border-radius: 4px;
		padding: 12px 16px;

		&.status-error {
			border-left-color: config.$bzdGraphColorRed;
		}
		&.status-running {
			border-left-color: config.$bzdGraphColorGreen;
		}
		&.status-starting,
		&.status-stopping {
			border-left-color: config.$bzdGraphColorOrange;
		}
		&.status-idle {
			border-left-color: config.$bzdGraphColorGray;
		}

		h2 {
			margin: 0 0 8px 0;
			font-size: 16px;
		}

		.service-header {
			display: flex;
			gap: 24px;
			padding: 4px 0 12px 0;
			.entry {
				white-space: nowrap;
				.key {
					color: config.$bzdGraphColorGray;
				}
			}
			code {
				font-family: monospace;
			}
		}
	}

	.status-error code,
	.process-row.status-error .status-text {
		color: config.$bzdGraphColorRed;
		font-weight: 600;
	}
	.status-running code,
	.process-row.status-running .status-text {
		color: config.$bzdGraphColorGreen;
	}
	.status-starting code,
	.process-row.status-starting .status-text,
	.status-stopping code,
	.process-row.status-stopping .status-text {
		color: config.$bzdGraphColorOrange;
	}
	.status-idle code,
	.process-row.status-idle .status-text {
		color: config.$bzdGraphColorGray;
	}

	table {
		width: 100%;
		border-collapse: collapse;
		font-size: 13px;
		th,
		td {
			padding: 6px 10px;
			text-align: left;
			vertical-align: top;
		}
		th {
			position: sticky;
			top: 0;
			z-index: 1;
			background-color: #f5f5f5;
			border-bottom: 1px solid color.adjust(config.$bzdGraphColorWhite, $lightness: -15%);
		}
		tbody tr {
			border-bottom: 1px solid color.adjust(config.$bzdGraphColorWhite, $lightness: -5%);
		}
		tbody tr:hover {
			background: color.adjust(config.$bzdGraphColorWhite, $lightness: -3%);
		}
		tbody tr:nth-child(even):not(.process-error-row) {
			background: #fafafa;
		}
	}

	.process-row .status-text {
		font-weight: normal;
	}

	.process-error-row {
		td {
			background: color.adjust(config.$bzdGraphColorRed, $lightness: 45%);
			padding: 8px 10px;
		}
		.error-label {
			color: config.$bzdGraphColorRed;
			font-weight: 600;
			margin-right: 6px;
		}
		.error-message {
			font-family: monospace;
			white-space: pre-wrap;
			word-break: break-all;
		}
	}

	.error-count.has-errors {
		color: config.$bzdGraphColorRed;
		font-weight: 600;
	}

	.actions button {
		margin-right: 6px;
	}
	.actions button:disabled {
		opacity: 0.5;
		cursor: not-allowed;
	}

	.empty-state {
		text-align: center;
		padding: 20px;
		color: config.$bzdGraphColorGray;
	}

	.logs-panel {
		margin-top: 16px;
		border: 1px solid color.adjust(config.$bzdGraphColorWhite, $lightness: -15%);
		border-radius: 4px;

		.logs-header {
			display: flex;
			justify-content: space-between;
			align-items: center;
			padding: 8px 12px;
			background: #f5f5f5;
			border-bottom: 1px solid color.adjust(config.$bzdGraphColorWhite, $lightness: -10%);
			h2 {
				margin: 0;
				font-size: 14px;
			}
		}

		.logs-close {
			/* minimal restyle; the global button styling comes from the app-level theme */
		}

		.log-output {
			font-family: monospace;
			white-space: pre-wrap;
			word-break: break-all;
		}

		.log-row {
			&.log-error {
				color: config.$bzdGraphColorRed;
			}
			&.log-error .log-output {
				font-weight: 600;
			}
			&.log-completed {
				color: config.$bzdGraphColorGreen;
			}
			&.log-running {
				color: config.$bzdGraphColorOrange;
			}
		}
	}
</style>
