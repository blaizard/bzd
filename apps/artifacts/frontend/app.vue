<template>
	<Layout :full-page="true">
		<template #header> Artifacts </template>
		<template #actions>
			<MenuEntry text="Services" icon="bzd-icon-tile" link="/services"></MenuEntry>
			<MenuEntry text="Add" icon="bzd-icon-add" link="/config"></MenuEntry>
		</template>
		<template #content>
			<div class="layout">
				<div class="tree">
					<Tree :key="refreshCounter" @item="handleItem"></Tree>
				</div>
				<div class="content">
					<RouterComponent ref="view" class="bzd-content"></RouterComponent>
				</div>
			</div>
		</template>
	</Layout>
</template>

<script>
	import DirectiveTooltip from "bzd/vue/directives/tooltip.mjs";
	import Layout from "bzd/vue/components/layout/layout.vue";
	import Tree from "./tree.vue";
	import MenuEntry from "bzd/vue/components/menu/entry.vue";

	export default {
		components: {
			Layout,
			Tree,
			MenuEntry
		},
		directives: {
			tooltip: DirectiveTooltip
		},
		data: function() {
			return {
				refreshCounter: 0,
				path: []
			};
		},
		mounted() {
			this.$routerSet({
				ref: "view",
				routes: [
					{ path: "/", component: null },
					{
						path: "/refresh",
						handler: () => {
							++this.refreshCounter;
							this.$routerDispatch("/");
						}
					},
					{ path: "/config/{volume}", component: () => import("./config.vue") },
					{ path: "/config", component: () => import("./config.vue") },
					{ path: "/view/{path:*}", component: () => import("./view.vue") },
					{ path: "/services", component: () => import("./services.vue") }
				]
			});
		},
		methods: {
			handleItem(item) {
				this.path = item.path.concat([item.item.name]);
				this.$routerDispatch("/view/" + this.path.map((c) => encodeURIComponent(c)).join("/"));
			}
		}
	};
</script>

<style lang="scss">
	@use "bzd-style/css/base.scss";
	@use "bzd-style/css/tooltip.scss";
	@use "bzd-style/css/loading.scss";

	@use "bzd/icons.scss" as icons with ($bzdIconNames: add tile);

	.bzd-content {
		margin: 10px;
	}
</style>

<style lang="scss" scoped>
	.layout {
		height: 100%;
		width: 100%;
		padding: 15px;
		display: flex;
		flex-flow: row wrap;
		> * {
			border: 1px solid #ddd;
			height: 100%;
			overflow: auto;
		}
		.tree {
			width: 400px;
			margin-right: 15px;
			padding: 10px;
		}
		.content {
			flex: 1;
		}
	}
</style>
