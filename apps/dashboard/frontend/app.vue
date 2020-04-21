<template>
	<Layout>
		<template #header>Dashboard</template>
		<template #actions>
			<MenuEntry text="Edit" icon="bzd-icon-edit" @click=""></MenuEntry>
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
			<RouterLink link="/dashboard/edit">/dashboard/edit</RouterLink>
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
	import API from "[bzd]/core/api.js";
	import APIv1 from "[dashboard]/api.v1.json";

	console.log("api v1", APIv1);

	const api = new API(APIv1);

	export default {
		components: {
			Layout, MenuEntry
		},
		directives: {
			"tooltip": DirectiveTooltip
		},
		data: function () {
			return {
			/*	router: this.$bzdRouter({
					ref: "foo",
					routes: [
						{ path: "/dashboard", nested: true, component: () => import("[frontend]/dashboard.vue") },
						{ path: "/add", component: () => import("[frontend]/add_tile.vue") }
					],
					fallback: { component: () => import("[frontend]/404.vue") }
				})*/
			}
		},
		mounted() {
			this.$registerRouter({
				ref: "foo",
				routes: [
					{ path: "/dashboard", nested: true, component: () => import("[frontend]/dashboard.vue") },
					{ path: "/add", component: () => import("[frontend]/add_tile.vue") }
				],
				fallback: { component: () => import("[frontend]/404.vue") }
			});
		},
		methods: {
			async test() {
				const response = await api.request("get", "/configuration");
				console.log(response);
			}
		}
	}
</script>

<style lang="scss">
	@import "~[bzd]/assets/style/base.scss";
	@import "~[bzd]/assets/style/tooltip.scss";

	$bzdIconNames: edit;
	@import "~[bzd]/icons.scss";
</style>
