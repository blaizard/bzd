<template>
	<Application>
		<template #menu> </template>
		<template #content>
			<RouterComponent name="view" class="bzd-content"></RouterComponent>
		</template>
	</Application>
</template>

<script>
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import Application from "#bzd/nodejs/vue/apps/application.vue";
	import MenuEntry from "#bzd/nodejs/vue/components/menu/entry.vue";

	export default {
		components: {
			Application,
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
				routes: [
					{ path: "/", component: () => import("./main.vue") },
					{ path: "/job/{job}", component: () => import("./job.vue") },
				],
			});
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/icons.scss" as icons;

	.bzd-icon-main {
		@include icons.defineIcon("@/apps/job_executor/frontend/icon.svg");
	}

	.bzd-content {
		padding: 20px;
	}
</style>
