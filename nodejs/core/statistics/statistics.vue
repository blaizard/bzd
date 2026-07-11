<template>
	<div class="bzd-statistics">
		<div class="bzd-statistics-search">
			<Input :description="{ placeholder: 'Filter...' }" @directInput="" />
		</div>
		<ViewData :pathList="pathList" :endpoint="endpoint"></ViewData>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import Input from "#bzd/nodejs/vue/components/form/element/input.vue";
	import ViewData from "#bzd/nodejs/db/data/frontend/view_data.vue";

	export default {
		mixins: [Component],
		components: {
			Input,
			ViewData,
		},
		data: function () {
			return {
				pathList: ["statistics"],
			};
		},
		computed: {
			endpoint() {
				return this.$rest.getEndpoint("/admin/statistics");
			},
		},
	};
</script>

<style lang="scss" scoped>
	@use "@/config.scss" as config;

	.bzd-statistics {
		.bzd-statistics-search {
			margin-bottom: 16px;
		}
	}
</style>
