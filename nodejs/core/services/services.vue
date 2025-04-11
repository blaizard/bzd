<template>
	<div>
		<div v-for="(data, uid) in services" class="services">
			<div class="service">
				<h2>{{ uid }}</h2>
				<div class="header">
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
					<tr>
						<th>Process</th>
						<th>Status</th>
						<th># Executions</th>
						<th># Errors</th>
						<th>Min</th>
						<th>Max</th>
						<th>Average</th>
						<th>Interval</th>
						<th></th>
					</tr>
					<tr v-for="(process, name) in data.processes">
						<td>{{ uid }}.{{ name }}</td>
						<td>{{ process.status }}</td>
						<td>{{ process.executions }}</td>
						<td>{{ process.errors }}</td>
						<td>{{ durationSToString(process.durationMin) }}</td>
						<td>{{ durationSToString(process.durationMax) }}</td>
						<td>{{ durationSToString(process.durationAvg) }}</td>
						<td>{{ process.estimatedInterval === 0 ? "-" : durationSToString(process.estimatedInterval) }}</td>
						<td>
							<button @click="processTrigger(uid, name)">Trigger</button>
							<button @click="processShowLogs(uid, name)">Show Logs</button>
						</td>
					</tr>
				</table>
			</div>
		</div>
		<template v-if="logs">
			<h2>Logs for {{ logs.uid }}.{{ logs.name }}</h2>
			<table>
				<tr>
					<th>Start</th>
					<th>Status</th>
					<th>Duration</th>
					<th>Output</th>
				</tr>
				<tr v-for="log in logs.logs">
					<td>{{ timestampToString(log.timestampStart) }}</td>
					<td>{{ logToStatus(log) }}</td>
					<td>{{ logToDuration(log) }}</td>
					<td>{{ logToOutput(log) }}</td>
				</tr>
			</table>
		</template>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import { timeMsToString, dateToDefaultString } from "#bzd/nodejs/utils/to_string.mjs";

	export default {
		mixins: [Component],
		mounted() {
			this.handleFetch();
		},
		data: function () {
			return {
				services: {},
				logs: null,
			};
		},
		computed: {},
		methods: {
			timestampToDurationString(timestamp) {
				if (!timestamp) {
					return "-";
				}
				const durationMs = this.timestamp - timestamp;
				return timeMsToString(durationMs);
			},
			durationSToString(durationS) {
				return timeMsToString(durationS * 1000);
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
				return timeMsToString(log.timestampStop - log.timestampStart);
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
			async handleFetch() {
				await this.handleSubmit(async () => {
					const result = await this.$rest.request("get", "/admin/services");
					this.timestamp = result.timestamp;
					this.services = result.services;
				});
			},
			async processShowLogs(uid, name) {
				await this.handleSubmit(async () => {
					const result = await this.$rest.request("get", "/admin/service/logs", { uid, name });
					this.logs = {
						uid: uid,
						name: name,
						logs: result.logs,
					};
				});
			},
			async processTrigger(uid, name) {
				await this.handleSubmit(async () => {
					await this.$rest.request("post", "/admin/service/trigger", { uid, name });
				});
			},
		},
	};
</script>

<style lang="scss" scoped>
	.services {
		.service {
			.header {
				columns: 4 193px;
				column-gap: 4em;
				padding: 5px 20px 20px 20px;

				.entry {
					white-space: nowrap;
				}
			}
		}
	}
</style>
