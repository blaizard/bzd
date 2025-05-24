<template>
	<Application>
		<template #content>
			<RouterComponent name="view" class="bzd-content"></RouterComponent>
		</template>
	</Application>
</template>

<script>
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import Application from "#bzd/nodejs/vue/apps/application.vue";

	export default {
		components: {
			Application,
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
	.bzd-content {
		padding: 20px;
	}
</style>
