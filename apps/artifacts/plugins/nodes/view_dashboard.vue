<template>
	<div class="view-dashboard">
		<Form :description="formDescription" v-model="options"></Form>
		<div class="components">
			<template v-for="dashboard in dashboards">
				<ViewGraph :title="dashboard.title" :inputs="dashboardInputs(dashboard)"></ViewGraph>
			</template>
		</div>
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import ViewGraph from "#bzd/apps/artifacts/plugins/nodes/view_graph.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import Utils from "#bzd/apps/artifacts/common/utils.mjs";

	export default {
		mixins: [Base, Component],
		components: {
			ViewGraph,
			Form,
		},
		data: function () {
			return {
				dashboards: [],
				// Timestamp difference between server and client.
				timestampDiff: 0,
				inputs: {},
				options: {
					interval: "Last 15 minutes",
				},
			};
		},
		mounted() {
			this.fetchDashboards();
		},
		computed: {
			dashboardEndpoint() {
				const [volume, ...key] = this.pathList;
				return "/x/" + encodeURIComponent(volume) + "/@dashboards" + Utils.keyToPath(key);
			},
			endpoint() {
				const [volume, uid, ..._] = this.pathList;
				return "/x/" + encodeURIComponent(volume) + "/" + encodeURIComponent(uid);
			},
			// Gather all inputs to gather.
			inputsKeys() {
				return [
					...new Set(
						this.dashboards
							.map((dashboard) => {
								return Object.keys(dashboard.inputs);
							})
							.flat(),
					),
				];
			},
			formDescription() {
				return [
					{
						type: "Dropdown",
						name: "interval",
						list: [
							"Last 5 minutes",
							"Last 15 minutes",
							"Last 30 minutes",
							"Last 1 hour",
							"Last 3 hours",
							"Last 6 hours",
							"Last 12 hours",
							"Last 24 hours",
							"Last 2 days",
							"Last 7 days",
							"Last 30 days",
							"Last 6 months",
							"Last 1 year",
							"Last 2 years",
							"Last 5 years",
						],
					},
				];
			},
		},
		methods: {
			dashboardInputs(dashboard) {
				let inputs = {};
				for (const [input, options] of Object.entries(dashboard.inputs)) {
					if (input in this.inputs) {
						inputs[input] = {
							data: this.inputs[input],
							options: options || {},
						};
					}
				}
				return inputs;
			},
			timeToServer(timestamp) {
				return timestamp + this.timestampDiff;
			},
			async fetchDashboards() {
				await this.handleSubmit(async () => {
					const result = await this.requestBackend(this.dashboardEndpoint, {
						method: "get",
						expect: "json",
					});
					this.dashboards = result.dashboards;
					this.timestampDiff = result.timestamp - Date.now();
				});
				await this.fetchData();
			},
			async fetchData() {
				await this.handleSubmit(async () => {
					const result = await this.requestBackend(this.endpoint, {
						method: "get",
						query: {
							include: this.inputsKeys.join(","),
							metadata: 1,
							count: 100,
							before: this.timeToServer(Date.now()),
							after: this.timeToServer(Date.now() - 30 * 24 * 3600 * 1000),
						},
						expect: "json",
					});

					let inputs = {};
					for (const [key, data] of result.data) {
						inputs[Utils.keyToPath(key)] = data;
					}
					this.inputs = inputs;
				});
			},
		},
	};
</script>

<style lang="scss" scoped>
	.components {
		display: flex;
		flex-direction: row;
		flex-wrap: wrap;
	}
</style>
