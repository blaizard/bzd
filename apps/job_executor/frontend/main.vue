<template>
	<div>
		<h1>Job</h1>
		<Form
			v-loading="loading"
			v-model="value"
			:description="formDescription"
			@submit="handleSubmitInputs"
			:all="true"
		></Form>
		<TerminalWebsocket style="height: 600px"></TerminalWebsocket>
	</div>
</template>

<script>
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import Jobs from "#bzd/apps/job_executor/jobs.json" with { type: "json" };
	import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.mjs";
	import TerminalWebsocket from "#bzd/nodejs/vue/components/terminal/frontend/websocket.vue";

	const Exception = ExceptionFactory("main");

	export default {
		mixins: [Component],
		components: {
			Form,
			TerminalWebsocket,
		},
		directives: {
			loading: DirectiveLoading,
		},
		data: function () {
			return {
				value: {},
			};
		},
		computed: {
			jobId() {
				return this.value["jobId"] ?? null;
			},
			jobDescription() {
				const jobList = Object.keys(Jobs);
				return { type: "Dropdown", list: jobList, name: "jobId" };
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
		},
		methods: {
			async handleSubmitInputs() {
				await this.handleSubmit(async () => {
					await this.$rest.request("post", "/job/send", {
						id: this.jobId,
						data: this.value,
					});
					this.value = {};
				});
			},
		},
	};
</script>

<style lang="scss"></style>
