<template>
	<Layout>
		<template #header>Dashboard</template>
		<template #actions>
			<MenuEntry text="Add new tile" icon="bzd-icon-add" link="/new"></MenuEntry>
			<MenuEntry text="Edit" icon="bzd-icon-edit" link="/edit"></MenuEntry>
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
			<RouterLink link="/">dashboard</RouterLink>
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
				edit: false
			}
		},
		mounted() {
			this.$routerSet({
				ref: "foo",
				routes: [
                    { path: '/', component: () => import("[frontend]/tiles.vue") },
                    { path: '/new', component: () => import("[frontend]/config.vue") },
                    { path: '/edit', handler: () => { this.edit = true; } },
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
	@import "~[bzd-style]/css/base.scss";
	@import "~[bzd-style]/css/tooltip.scss";

	$bzdIconNames: edit, add;
	@import "~[bzd]/icons.scss";
</style>
