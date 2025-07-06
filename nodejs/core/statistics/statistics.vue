<template>
	<div class="statistics">
		<pre>
			{{ JSON.stringify(statistics, null, 4) }}
		</pre
		>
	</div>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";

	export default {
		mixins: [Component],
		mounted() {
			this.handleFetch();
		},
		data: function () {
			return {
				statistics: {},
			};
		},
		computed: {},
		methods: {
			async handleFetch() {
				await this.handleSubmit(async () => {
					const result = await this.$rest.request("get", "/admin/statistics");
					this.statistics = result.data;
				});
			},
		},
	};
</script>

<style lang="scss" scoped></style>
