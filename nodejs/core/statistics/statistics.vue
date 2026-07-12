<template>
	<div class="bzd-statistics">
		<ViewData :path-list="pathList" :api-get="apiGet"></ViewData>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import ViewData from "#bzd/nodejs/db/data/frontend/view_data.vue";

	export default {
		mixins: [Component],
		components: {
			ViewData,
		},
		data: function () {
			return {
				pathList: ["statistics"],
			};
		},
		methods: {
			async apiGet(query) {
				const data = await this.$rest.request("get", "/admin/statistics", query);
				return JSON.parse(data);
			},
		},
	};
</script>

<style lang="scss" scoped>
	@use "@/config.scss" as config;
</style>
