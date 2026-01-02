<template>
	<div>
		<h1>Jobs</h1>
		<table class="jobs" v-loading="beforeFetchJobs">
			<tr>
				<th>Id</th>
				<th>Type</th>
				<th>Status</th>
				<th>Duration</th>
				<th>Arguments</th>
				<th></th>
			</tr>
			<tr v-for="(info, jobId) in jobs" class="job" @click="goToJob(jobId)">
				<td class="job-id">{{ jobId }}</td>
				<td class="type">{{ info.type }}</td>
				<td class="status">{{ getStatus(info) }}</td>
				<td class="duration">{{ getDuration(info.timestampStart, info.timestampStop) }}</td>
				<td class="args">{{ info.args ? info.args.join(" ") : "" }}</td>
				<td class="actions">
					<a v-if="isStart(jobId)" @click.stop="start(jobId)" v-tooltip="{ type: 'text', data: 'Start this job' }"
						><i class="bzd-icon-play"></i
					></a>
					<a v-if="isKill(jobId)" @click.stop="kill(jobId)" v-tooltip="{ type: 'text', data: 'Kill this job' }"
						><i class="bzd-icon-close"></i
					></a>
					<a @click.stop="goToJob(jobId)" v-tooltip="{ type: 'text', data: 'Access the shell' }"
						><i class="bzd-icon-shell"></i
					></a>
					<a @click.stop="remove(jobId)" v-tooltip="{ type: 'text', data: 'Remove this job' }"
						><i class="bzd-icon-trash"></i
					></a>
				</td>
			</tr>
		</table>

		<h1>Create a new job</h1>
		<Form
			v-loading="loading"
			v-model="value"
			:description="formDescription"
			@submit="handleSubmitInputs"
			:all="true"
		></Form>
	</div>
</template>

<script>
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import Jobs from "#bzd/apps/job_executor/jobs.json" with { type: "json" };
	import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
	import LogFactory from "#bzd/nodejs/core/log.mjs";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.mjs";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import { timeMsToString } from "#bzd/nodejs/utils/to_string.mjs";
	import Status from "#bzd/apps/job_executor/backend/status.mjs";

	const Exception = ExceptionFactory("main");
	const Log = LogFactory("main");

	export default {
		mixins: [Component],
		components: {
			Form,
		},
		directives: {
			loading: DirectiveLoading,
			tooltip: DirectiveTooltip,
		},
		mounted() {
			this.fetchJobs();
		},
		beforeUnmount() {
			clearTimeout(this.instanceTimeout);
		},
		data: function () {
			return {
				value: {},
				jobs: {},
				timestampServer: null,
				instanceTimeout: null,
			};
		},
		computed: {
			jobId() {
				return this.value["jobId"] ?? null;
			},
			jobDescription() {
				const jobList = Object.keys(Jobs);
				return {
					type: "Dropdown",
					list: jobList,
					name: "jobId",
					caption: "Type",
					onchange: (value) => {
						this.value = Object.assign({}, Jobs[this.jobId].default, this.value);
					},
				};
			},
			formDescription() {
				let description = [this.jobDescription];
				if (!this.jobId) {
					return description;
				}
				Exception.assert(this.jobId in Jobs, `Job ${this.jobId} not found`);
				description = [...description, ...Jobs[this.jobId].inputs, { type: "Button", action: "approve" }];
				return description;
			},
			beforeFetchJobs() {
				return this.instanceTimeout === null;
			},
		},
		methods: {
			async fetchJobs() {
				try {
					const data = await this.$rest.request("get", "/jobs");
					this.jobs = data.jobs;
					this.timestampServer = data.timestamp;
				} finally {
					this.instanceTimeout = setTimeout(this.fetchJobs, 1000);
				}
			},
			async handleSubmitInputs() {
				await this.handleSubmit(async () => {
					const response = await this.$rest.request("post", "/job/send", {
						id: this.jobId,
						data: this.value,
					});
					this.goToJob(response.job);
				});
			},
			isStart(id) {
				return this.jobs[id].status == "idle";
			},
			isKill(id) {
				return this.jobs[id].status == "running";
			},
			async start(id) {
				await this.handleSubmit(async () => {
					await this.$rest.request("post", "/job/{id}/start", {
						id: id,
					});
				});
			},
			async kill(id) {
				await this.handleSubmit(async () => {
					await this.$rest.request("delete", "/job/{id}/cancel", {
						id: id,
					});
				});
			},
			async remove(id) {
				await this.handleSubmit(async () => {
					await this.$rest.request("delete", "/job/{id}/delete", {
						id: id,
					});
				});
			},
			goToJob(id) {
				this.$router.dispatch("/job/" + id);
				this.value = {};
			},
			getDuration(timestampStart, timestampStop) {
				if (!timestampStart) {
					return "-";
				}
				const durationMs = (timestampStop === null ? this.timestampServer : timestampStop) - timestampStart;
				return timeMsToString(durationMs);
			},
			getStatus(info) {
				if (info.status == Status.idle && info.scheduler?.type) {
					return "scheduled (" + info.scheduler.type + ")";
				}
				return info.status;
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/icons.scss" as icons with (
		$bzdIconNames: close shell trash play
	);
</style>

<style lang="scss" scoped>
	.jobs {
		.job {
			&:hover {
				background-color: #eee;
				cursor: pointer;
			}
			.actions {
				width: 0;

				> * {
					display: inline-block;
					margin-right: 5px;
				}
			}
		}
	}
</style>
