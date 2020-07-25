<template>
	<Layout>
		<template #header><RouterLink link="/">Showcase</RouterLink></template>
		<template #menu>
			<MenuEntry text="Colors" link="/colors"></MenuEntry>
			<MenuEntry text="Notification" link="/notification"></MenuEntry>
			<MenuEntry text="Graph">
				<MenuEntry text="Plot" link="/graph/plot"></MenuEntry>
			</MenuEntry>
			<MenuEntry text="Form" link="/form">
				<MenuEntry text="Standalone" link="/form/standalone"></MenuEntry>
				<MenuEntry text="Input" link="/form/input"></MenuEntry>
				<MenuEntry text="Password" link="/form/password"></MenuEntry>
				<MenuEntry text="Dropdown" link="/form/dropdown"></MenuEntry>
				<MenuEntry text="Autocomplete" link="/form/autocomplete"></MenuEntry>
				<MenuEntry text="Validation" link="/form/validation"></MenuEntry>
				<MenuEntry text="Output" link="/form/output"></MenuEntry>
			</MenuEntry>
		</template>
		<template #content>
			<RouterComponent ref="view"></RouterComponent>
		</template>
	</Layout>
</template>

<script>
import Layout from "bzd/vue/components/layout/layout.vue";
import MenuEntry from "bzd/vue/components/menu/entry.vue";

export default {
	components: {
		Layout,
		MenuEntry,
	},
	data: function () {
		return {};
	},
	mounted() {
		this.$routerSet({
			ref: "view",
			routes: [
				{ path: "/colors", component: () => import("./colors.vue") },
				{ path: "/notification", component: () => import("./notification.vue") },
				{ path: "/graph/plot", component: () => import("./graph_plot.vue") },
				{ path: "/form", component: () => import("./form/form.vue") },
				{ path: "/form/standalone", component: () => import("./form/standalone.vue") },
				{ path: "/form/input", component: () => import("./form/input.vue") },
				{ path: "/form/password", component: () => import("./form/password.vue") },
				{ path: "/form/dropdown", component: () => import("./form/dropdown.vue") },
				{ path: "/form/autocomplete", component: () => import("./form/autocomplete.vue") },
				{ path: "/form/validation", component: () => import("./form/validation.vue") },
				{ path: "/form/output", component: () => import("./form/output.vue") },
			],
		});
	},
};
</script>

<style lang="scss">
	@use "bzd-style/css/base.scss";
	@use "bzd-style/css/tooltip.scss";
	@use "bzd-style/css/loading.scss";
</style>
