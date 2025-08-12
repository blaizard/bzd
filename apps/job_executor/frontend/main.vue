<template>
	<div>
		<h1>Jobs</h1>
		<table class="jobs">
			<tr>
				<th>Id</th>
				<th>Type</th>
				<th>Status</th>
				<th>Duration</th>
				<th>Arguments</th>
				<th></th>
			</tr>
			<tr v-for="(info, jobId) in jobs" class="job">
				<td class="job-id">{{ jobId }}</td>
				<td class="type">{{ info.type }}</td>
				<td class="status">{{ info.status }}</td>
				<td class="duration">{{ getDuration(info.timestampStart, info.timestampStop) }}</td>
				<td class="args">{{ info.args.join(" ") }}</td>
				<td class="actions">
					<a v-if="isKill(jobId)" @click="kill(jobId)" v-tooltip="tooltipKill"><i class="bzd-icon-close"></i></a>
					<a @click="goToJob(jobId)" v-tooltip="tooltipShell"><i class="bzd-icon-shell"></i></a>
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
				return { type: "Dropdown", list: jobList, name: "jobId", caption: "Type" };
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
			tooltipShell() {
				return {
					type: "text",
					data: "Access shell",
				};
			},
			tooltipKill() {
				return {
					type: "text",
					data: "Kill this job",
				};
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
			isKill(id) {
				return this.jobs[id].status == "running";
			},
			async kill(id) {
				await this.handleSubmit(async () => {
					await this.$rest.request("delete", "/job/{id}", {
						id: id,
					});
				});
			},
			goToJob(id) {
				this.$router.dispatch("/job/" + id);
				this.value = {};
			},
			getDuration(timestampStart, timestampStop) {
				if (timestampStart === null) {
					return "-";
				}
				const durationMs = (timestampStop === null ? this.timestampServer : timestampStop) - timestampStart;
				return timeMsToString(durationMs);
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/icons.scss" as icons with (
		$bzdIconNames: close shell
	);
</style>

<style lang="scss" scoped>
	.jobs {
		.job {
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
