<template>
	<div>
		<Form :description="formDescription" v-model="options"></Form>
		<ViewDashboard v-if="options.dashboard" :pathList="pathList"></ViewDashboard>
		<ViewData v-else :pathList="pathList"></ViewData>
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import ViewData from "#bzd/apps/artifacts/plugins/nodes/frontend/view_data.vue";
	import ViewDashboard from "#bzd/apps/artifacts/plugins/nodes/frontend/view_dashboard.vue";
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
				return [{ type: "Checkbox", name: "dashboard", text: "Dashboard", align: "right" }];
			},
		},
	};
</script>
