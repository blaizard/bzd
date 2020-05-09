<template>
	<Layout>
		<template #header><RouterLink link="/">Showcase</RouterLink></template>
		<template #menu>
			<MenuEntry text="Graph">
				<MenuEntry text="Plot" link="/graph/plot"></MenuEntry>
			</MenuEntry>
			<MenuEntry text="Form" link="/form">
				<MenuEntry text="Standalone" link="/form/standalone"></MenuEntry>
			</MenuEntry>
		</template>
		<template #content>
			<RouterComponent ref="view"></RouterComponent>
		</template>
	</Layout>
</template>

<script>
	"use strict"

	import Layout from "bzd/vue/components/layout/layout.vue";
	import MenuEntry from "bzd/vue/components/menu/entry.vue";

	export default {
		components: {
			Layout, MenuEntry
		},
		data: function () {
			return {
			}
		},
		mounted() {
			this.$routerSet({
				ref: "view",
				routes: [
                    { path: '/graph/plot', component: () => import("./graph_plot.vue") },
                    { path: '/form', component: () => import("./form.vue") },
                    { path: '/form/standalone', component: () => import("./form_standalone.vue") },
				],
			});
		},
	}
</script>

<style lang="scss">
	@import "bzd-style/css/base.scss";
	@import "bzd-style/css/tooltip.scss";
	@import "bzd-style/css/loading.scss";
</style>
