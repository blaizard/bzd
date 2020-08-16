<template>
	<Layout :full-page="true">
		<template #header>
			Artifacts
		</template>
		<template #content>
			<div class="layout">
				<div class="tree">
					<Tree @config="handleConfig"></Tree>
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
	import Plugins from "../plugins/frontend.mjs";

	export default {
		components: {
			Layout,
			Tree
		},
		directives: {
			tooltip: DirectiveTooltip
		},
		data: function() {
			return {};
		},
		mounted() {
			this.$routerSet({
				ref: "view",
				routes: [{ path: "/config", component: () => import("./config.vue") }]
			});
			console.log(Plugins);
		},
		methods: {
			handleConfig(volume) {
				this.$routerDispatch("/config", {
					volume: volume
				});
			}
		}
	};
</script>

<style lang="scss">
	@use "bzd-style/css/base.scss";
	@use "bzd-style/css/tooltip.scss";
	@use "bzd-style/css/loading.scss";

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
