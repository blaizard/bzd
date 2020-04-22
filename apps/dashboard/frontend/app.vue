<template>
	<Layout>
		<template #header>Dashboard</template>
		<template #actions>
			<MenuEntry text="Add new tile" icon="bzd-icon-add" link="/dashboard/new"></MenuEntry>
			<MenuEntry text="Edit" icon="bzd-icon-edit" link="/dashboard/edit"></MenuEntry>
		</template>
		<template #menu>
			<MenuEntry text="About" icon="bzd-icon-edit">
				<MenuEntry text="One" icon="bzd-icon-edit"></MenuEntry>
				<MenuEntry text="Two" icon="bzd-icon-edit"></MenuEntry>
				<MenuEntry text="Three" icon="bzd-icon-edit"></MenuEntry>
			</MenuEntry>
		</template>
		<template #content>
			<span v-tooltip="{'text': 'Hello world'}">Content</span>
			<RouterComponent ref="foo"></RouterComponent>
			<RouterLink link="/add">/add</RouterLink>
			<RouterLink link="/dashboard">/dashboard</RouterLink>
		</template>
		<template #footer>
			Footer <button @click="test">Test</button>
		</template>
	</Layout>
</template>

<script>
	"use strict"

	import Layout from "[frontend]/layout.vue";
	import MenuEntry from "[frontend]/menu_entry.vue";
	import DirectiveTooltip from "[bzd]/vue/directives/tooltip.js"
	import Fetch from "[bzd]/core/fetch.js"

	export default {
		components: {
			Layout, MenuEntry
		},
		directives: {
			"tooltip": DirectiveTooltip
		},
		data: function () {
			return {
			}
		},
		mounted() {
			this.$routerSet({
				ref: "foo",
				routes: [
					{ path: "/dashboard", nested: true, component: () => import("[frontend]/dashboard.vue") },
					{ path: "/add", component: () => import("[frontend]/config.vue") }
				],
				fallback: { component: () => import("[frontend]/404.vue") }
			});
		},
		methods: {
			async test() {
				const response = await this.$api.request("get", "/configuration");
				console.log(response);
			}
		}
	}
</script>

<style lang="scss">
	@import "~[bzd]/assets/style/base.scss";
	@import "~[bzd]/assets/style/tooltip.scss";

	$bzdIconNames: edit, add;
	@import "~[bzd]/icons.scss";
</style>
