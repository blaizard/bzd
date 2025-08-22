<template>
	<div class="components">
		<template v-for="dashboard in dashboards">
			<ViewGraph :title="dashboard.title" :inputs="dashboardInputs(dashboard)"></ViewGraph>
		</template>
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import ViewGraph from "#bzd/apps/artifacts/plugins/nodes/view_graph.vue";

	export default {
		mixins: [Base, Component],
		components: {
			ViewGraph,
		},
		data: function () {
			return {
				dashboards: [],
				inputs: {},
			};
		},
		mounted() {
			this.fetchDashboards();
		},
		computed: {
			dashboardEndpoint() {
				const [volume, ...rest] = this.pathList;
				return "/x/" + encodeURIComponent(volume) + "/@dashboards/" + rest.map(encodeURIComponent).join("/");
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
			async fetchDashboards() {
				await this.handleSubmit(async () => {
					const result = await this.requestBackend(this.dashboardEndpoint, {
						method: "get",
						expect: "json",
					});
					this.dashboards = result.dashboards;
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
						},
						expect: "json",
					});

					let inputs = {};
					for (const [key, data] of result.data) {
						const path = "/" + key.map(encodeURIComponent).join("/");
						inputs[path] = data;
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
