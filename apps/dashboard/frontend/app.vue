<template>
	<Layout>
		<template #header><RouterLink link="/"><i class="bzd-icon-main"></i> Dashboard</RouterLink></template>
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


import DirectiveTooltip from "bzd/vue/directives/tooltip.mjs";
import Layout from "bzd/vue/components/layout/layout.vue";
import MenuEntry from "bzd/vue/components/menu/entry.vue";

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
		};
	},
	mounted() {
		this.$routerSet({
			ref: "view",
			routes: [
				{ path: "/", component: () => import("./tiles.vue") },
				{ path: "/new", component: () => import("./config.vue") },
				{ path: "/update/{uid}", component: () => import("./config.vue") },
			],
			fallback: { component: () => import("./404.vue") }
		});
	},
	methods: {
		handleEdit() {
			this.edit = !this.edit;
		},
	}
};
</script>

<style lang="scss">
	@use "bzd-style/css/base.scss";
	@use "bzd-style/css/tooltip.scss";
	@use "bzd-style/css/loading.scss";

	@use "bzd/icons.scss" as icons with (
		$bzdIconNames: edit add check
	);

    .bzd-icon-main {
		@include icons.defineIcon("icon.svg");
	}
</style>
