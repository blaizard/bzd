<template>
	<Layout>
		<template #header><RouterLink link="/">Dashboard</RouterLink></template>
		<template #actions>
			<MenuEntry text="Add new tile" icon="bzd-icon-add" link="/new"></MenuEntry>
			<MenuEntry v-if="!edit" text="Edit" icon="bzd-icon-edit" @click="handleEdit"></MenuEntry>
			<MenuEntry v-else text="OK" icon="bzd-icon-check" @click="handleEdit"></MenuEntry>
		</template>
		<template #menu>
			<MenuEntry text="About" icon="bzd-icon-edit">
				<MenuEntry text="One" icon="bzd-icon-edit"></MenuEntry>
				<MenuEntry text="Two" icon="bzd-icon-edit"></MenuEntry>
				<MenuEntry text="Three" icon="bzd-icon-edit"></MenuEntry>
			</MenuEntry>
		</template>
		<template #content>
			<RouterComponent ref="view" :edit="edit"></RouterComponent>
		</template>
	</Layout>
</template>

<script>
	"use strict"

	import Layout from "[frontend]/layout.vue";
	import MenuEntry from "[frontend]/menu_entry.vue";
	import DirectiveTooltip from "[bzd]/vue/directives/tooltip.js"

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
				ref: "view",
				routes: [
                    { path: '/', component: () => import("[frontend]/tiles.vue") },
                    { path: '/new', component: () => import("[frontend]/config.vue") },
                    { path: '/update/{uid}', component: () => import("[frontend]/config.vue") },
				],
				fallback: { component: () => import("[frontend]/404.vue") }
			});
		},
		methods: {
			handleEdit() {
				this.edit = !this.edit;
			},
		}
	}
</script>

<style lang="scss">
	@import "~[bzd-style]/css/base.scss";
	@import "~[bzd-style]/css/tooltip.scss";
	@import "~[bzd-style]/css/loading.scss";

	$bzdIconNames: edit, add, check;
	@import "~[bzd]/icons.scss";
</style>
