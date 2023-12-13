<template>
	<Layout>
		<template #header><RouterLink link="/">Showcase</RouterLink></template>
		<template #menu>
			<MenuEntry text="Colors" link="/colors"></MenuEntry>
			<MenuEntry text="Router" link="/router"></MenuEntry>
			<MenuEntry text="Notification" link="/notification"></MenuEntry>
			<MenuEntry text="Tooltip" link="/tooltip"></MenuEntry>
			<MenuEntry text="Graph">
				<MenuEntry text="Plot" link="/graph/plot"></MenuEntry>
			</MenuEntry>
			<MenuEntry text="Form" link="/form">
				<MenuEntry text="Standalone" link="/form/standalone"></MenuEntry>
				<MenuEntry text="Condition" link="/form/condition"></MenuEntry>
				<MenuEntry text="Input" link="/form/input"></MenuEntry>
				<MenuEntry text="Password" link="/form/password"></MenuEntry>
				<MenuEntry text="Dropdown" link="/form/dropdown"></MenuEntry>
				<MenuEntry text="Autocomplete" link="/form/autocomplete"></MenuEntry>
				<MenuEntry text="Validation" link="/form/validation"></MenuEntry>
				<MenuEntry text="Output" link="/form/output"></MenuEntry>
			</MenuEntry>
		</template>
		<template #content>
			<RouterComponent name="view"></RouterComponent>
		</template>
	</Layout>
</template>

<script>
	import Layout from "#bzd/nodejs/vue/components/layout/layout.vue";
	import MenuEntry from "#bzd/nodejs/vue/components/menu/entry.vue";

	export default {
		components: {
			Layout,
			MenuEntry,
		},
		data: function () {
			return {};
		},
		mounted() {
			this.$router.set({
				component: "view",
				routes: [
					{ path: "/colors", component: () => import("./colors.vue") },
					{ path: "/router", component: () => import("./router.vue") },
					{ path: "/notification", component: () => import("./notification.vue") },
					{ path: "/graph/plot", component: () => import("./graph_plot.vue") },
					{ path: "/form", component: () => import("./form/form.vue") },
					{ path: "/form/standalone", component: () => import("./form/standalone.vue") },
					{ path: "/form/condition", component: () => import("./form/condition.vue") },
					{ path: "/form/input", component: () => import("./form/input.vue") },
					{ path: "/form/password", component: () => import("./form/password.vue") },
					{ path: "/form/dropdown", component: () => import("./form/dropdown.vue") },
					{ path: "/form/autocomplete", component: () => import("./form/autocomplete.vue") },
					{ path: "/form/validation", component: () => import("./form/validation.vue") },
					{ path: "/form/output", component: () => import("./form/output.vue") },
					{ path: "/tooltip", component: () => import("./tooltip.vue") },
				],
				fallback: { component: () => import("./404.vue") },
			});
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/styles/default/css/base.scss" as *;
	@use "#bzd/nodejs/styles/default/css/tooltip.scss" as *;
	@use "#bzd/nodejs/styles/default/css/loading.scss" as *;
</style>
