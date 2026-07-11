<template>
	<div>
		<Form :description="formDescription" v-model="options"></Form>
		<ViewDashboard v-if="options.dashboard" :pathList="pathList"></ViewDashboard>
		<ViewData v-else :pathList="pathList" :endpoint="endpoint" @select="onSelect"></ViewData>
		<div>
			<span>Export:</span>
			<Form :description="formDescriptionExport" v-model="exportOptions" @submit="handleExportSubmit"></Form>
			<span>REST API:</span>
			<ul>
				<li v-for="accessor in endpointsAccessors">
					<a :href="accessor">{{ accessor }}</a>
				</li>
			</ul>
		</div>
	</div>
</template>

<script>
	import Base from "#bzd/apps/artifacts/plugins/base.vue";
	import ViewData from "#bzd/nodejs/db/data/frontend/view_data.vue";
	import ViewDashboard from "#bzd/apps/artifacts/plugins/nodes/frontend/view_dashboard.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import LocalStorage from "#bzd/nodejs/core/localstorage.js";
	import Utils from "#bzd/apps/artifacts/common/utils.js";

	const optionsStorageKey = "bzd-artifacts-plugin-nodes-view-options";

	export default {
		mixins: [Base],
		components: {
			ViewData,
			ViewDashboard,
			Form,
		},
		data: function () {
			const now = new Date();
			const nowMinus1Year = now;
			nowMinus1Year.setFullYear(nowMinus1Year.getFullYear() - 1);
			return {
				options: {},
				exportOptions: {
					format: "csv",
					after: nowMinus1Year.getTime(),
					before: now.getTime(),
				},
			};
		},
		mounted() {
			try {
				this.options = LocalStorage.getSerializable(optionsStorageKey, this.options);
			} catch (e) {
				// ignore.
			}
		},
		watch: {
			options() {
				LocalStorage.setSerializable(optionsStorageKey, this.options);
			},
		},
		computed: {
			formDescription() {
				return [{ type: "Checkbox", name: "dashboard", text: "Dashboard", align: "right" }];
			},
			formDescriptionExport() {
				return [
					{
						type: "Date",
						name: "after",
						width: 0.3,
					},
					{
						type: "Date",
						name: "before",
						width: 0.3,
					},
					{
						type: "Dropdown",
						name: "format",
						list: ["csv"],
						width: 0.2,
					},
					{
						type: "Button",
						action: "approve",
						content: "Export",
						width: 0.2,
					},
				];
			},
			endpoint() {
				return "/x" + Utils.keyToPath(this.pathList);
			},
			endpointExport() {
				const [volume, ...path] = this.pathList;
				return "/x/" + encodeURIComponent(volume) + "/@export" + Utils.keyToPath(path);
			},
			endpointsAccessors() {
				return [
					this.endpoint + "?children=99",
					this.endpoint + "?children=99&metadata=1",
					this.endpoint + "?children=99&count=5",
					this.endpoint + "?keys=1",
					this.endpointExport + "?children=99&format=csv",
				];
			},
		},
		methods: {
			handleExportSubmit(values) {
				const query = ["children=99", ...Object.entries(values).map(([key, value]) => key + "=" + value)];
				const url = this.endpointExport + "?" + query.join("&");
				window.location.assign(url);
			},
			onSelect(pathList) {
				const url = "/view/" + Utils.keyToPath(pathList);
				this.$router.dispatch(url);
			},
		},
	};
</script>
