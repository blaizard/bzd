<template>
	<Layout>
		<template #header>Dashboard</template>
		<template #actions>
			<MenuEntry text="Edit" icon="bzd-icon-edit"></MenuEntry>
		</template>
		<template #menu>
			<MenuEntry text="About" icon="bzd-icon-edit">
				<MenuEntry text="One" icon="bzd-icon-edit"></MenuEntry>
				<MenuEntry text="Two" icon="bzd-icon-edit"></MenuEntry>
				<MenuEntry text="Three" icon="bzd-icon-edit"></MenuEntry>
			</MenuEntry>
			<router-link to="/add">Go to Frr</router-link>
			<router-link to="/dashboard">Go to Bar</router-link>
		</template>
		<template #content>
			<span v-tooltip="{'text': 'Hello world'}">Content</span>
			<router-view></router-view>
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

	const routesEntryPoint = {
		dashboard: () => import("[frontend]/dashboard.vue"),
		entry: () => import("[frontend]/add_tile.vue"),
	};

	export default {
		components: {
			Layout, MenuEntry
		},
		directives: {
			"tooltip": DirectiveTooltip
		},
		mounted() {
			
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
