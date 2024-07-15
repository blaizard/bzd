<template>
	<Layout>
		<template #header
			><RouterLink link="/"><i class="bzd-icon-main"></i> Dashboard</RouterLink>
		</template>
		<template #menu> </template>
		<template #content>
			<RouterComponent name="view" class="bzd-content"></RouterComponent>
		</template>
	</Layout>
</template>

<script>
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import Layout from "#bzd/nodejs/vue/components/layout/layout.vue";
	import MenuEntry from "#bzd/nodejs/vue/components/menu/entry.vue";

	export default {
		components: {
			Layout,
			MenuEntry,
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		data: function () {
			return {};
		},
		mounted() {
			this.$router.set({
				component: "view",
				routes: [{ path: "/", component: () => import("./instances.vue") }],
				fallback: { component: () => import("./404.vue") },
			});
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/styles/default/css/base.scss" as *;
	@use "#bzd/nodejs/styles/default/css/tooltip.scss" as *;
	@use "#bzd/nodejs/styles/default/css/loading.scss" as *;

	@use "#bzd/nodejs/icons.scss" as icons with (
		$bzdIconNames: add check
	);

	.bzd-icon-main {
		@include icons.defineIcon("@/apps/dashboard/frontend/icon.svg");
	}

	.bzd-content {
		padding: 20px;
	}
</style>
