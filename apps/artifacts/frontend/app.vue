<template>
	<Layout>
		<template #header>
			Artifacts
		</template>
		<template #content>
			<RouterComponent ref="view" class="bzd-content"></RouterComponent>
		</template>
	</Layout>
</template>

<script>
	import DirectiveTooltip from "bzd/vue/directives/tooltip.mjs";
	import Layout from "bzd/vue/components/layout/layout.vue";

	export default {
		components: {
			Layout
		},
		directives: {
			tooltip: DirectiveTooltip
		},
		data: function() {
			return {};
		},
		mounted() {
			this.$routerSet({
				ref: "view",
				routes: [{ path: "/", component: () => import("./home.vue") }]
			});
		}
	};
</script>

<style lang="scss">
	@use "bzd-style/css/base.scss";
	@use "bzd-style/css/tooltip.scss";
	@use "bzd-style/css/loading.scss";

	.bzd-content {
		margin: 10px;
	}
</style>
