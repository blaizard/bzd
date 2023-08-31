<template>
	<Layout>
		<template #header><RouterLink link="/"><i class="bzd-icon-main"></i> Dashboard</RouterLink></template>
		<template #actions>
			<MenuEntry text="Add new tile" icon="bzd-icon-add" link="/new"></MenuEntry>
			<MenuEntry v-if="!edit" text="Edit" icon="bzd-icon-edit" @click="handleEdit"></MenuEntry>
			<MenuEntry v-else text="OK" icon="bzd-icon-check" @click="handleEdit"></MenuEntry>
		</template>
		<template #menu> </template>
		<template #content>
			<RouterComponent ref="view" :edit="edit" class="bzd-content"></RouterComponent>
		</template>
	</Layout>
</template>

<script>
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import Layout from "#bzd/nodejs/vue/components/layout/layout.vue";
	import MenuEntry from "#bzd/nodejs/vue/components/menu/entry.vue";

	export default {
		components: {
			Layout,
			MenuEntry,
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		data: function () {
			return {
				edit: false,
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
				fallback: { component: () => import("./404.vue") },
			});
		},
		methods: {
			handleEdit() {
				this.edit = !this.edit;
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/styles/default/css/base.scss" as *;
	@use "#bzd/nodejs/styles/default/css/tooltip.scss" as *;
	@use "#bzd/nodejs/styles/default/css/loading.scss" as *;

	@use "#bzd/nodejs/icons.scss" as icons with (
		$bzdIconNames: edit add check
	);

	.bzd-icon-main {
		@include icons.defineIcon("@/apps/dashboard/frontend/icon.svg");
	}

	.bzd-content {
		padding: 20px;
	}
</style>
