<template>
	<Application>
		<template #content>
			<div class="layout">
				<div class="tree">
					<Tree :key="refreshCounter" @item="handleItem" :show-path="showPath"></Tree>
				</div>
				<div class="content">
					<RouterComponent name="view" class="bzd-content" @show="handleShowPath"></RouterComponent>
				</div>
			</div>
		</template>
	</Application>
</template>

<script>
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import Application from "#bzd/nodejs/vue/apps/application.vue";
	import Tree from "./tree.vue";
	import MenuEntry from "#bzd/nodejs/vue/components/menu/entry.vue";

	export default {
		components: {
			Application,
			Tree,
			MenuEntry,
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		data: function () {
			return {
				refreshCounter: 0,
				// This corresponds to the path to be shown by the tree view.
				showPath: [],
			};
		},
		mounted() {
			this.$router.set({
				component: "view",
				routes: [
					{ path: "/", component: null },
					{
						path: "/refresh",
						handler: () => {
							++this.refreshCounter;
							this.$router.dispatch("/");
						},
					},
					{ path: "/view/{path:*}", component: () => import("./view.vue") },
				],
			});
		},
		methods: {
			handleItem(item) {
				const path = item.path.concat([item.item.name]);
				this.$router.dispatch("/view/" + path.map((c) => encodeURIComponent(c)).join("/"));
			},
			handleShowPath(path) {
				this.showPath = path;
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/icons.scss" as icons with (
		$bzdIconNames: add tile
	);

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
			width: 25%;
			max-width: 400px;
			margin-right: 15px;
			padding: 10px;
		}
		.content {
			flex: 1;
		}
	}
</style>
