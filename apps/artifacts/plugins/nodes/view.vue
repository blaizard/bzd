<template>
	<div>
		<Form :description="formDescription" v-model="options"></Form>
		<ViewDashboard v-if="options.dashboard" :pathList="pathList"></ViewDashboard>
		<ViewData v-else :pathList="pathList"></ViewData>
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import ViewData from "#bzd/apps/artifacts/plugins/nodes/view_data.vue";
	import ViewDashboard from "#bzd/apps/artifacts/plugins/nodes/view_dashboard.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import LocalStorage from "#bzd/nodejs/core/localstorage.mjs";

	const optionsStorageKey = "bzd-artifacts-plugin-nodes-view-options";

	export default {
		mixins: [Base],
		components: {
			ViewData,
			ViewDashboard,
			Form,
		},
		data: function () {
			return {
				options: {},
			};
		},
		mounted() {
			try {
				this.options = JSON.parse(LocalStorage.get(optionsStorageKey, "{}"));
			} catch (e) {
				// ignore.
			}
		},
		watch: {
			options() {
				LocalStorage.set(optionsStorageKey, JSON.stringify(this.options));
			},
		},
		computed: {
			formDescription() {
				return [
					{ type: "Checkbox", name: "dashboard", text: "Dashboard", align: "right" },
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
						condition: (value) => value.dashboard,
					},
				];
			},
		},
	};
</script>
