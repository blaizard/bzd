<template>
	<Layout :full-page="true">
		<template #header>
			<a @click="handleHeader"><i class="bzd-icon-artifact"></i> Artifacts</a>
		</template>
		<template #actions>
			<MenuEntry text="Services" icon="bzd-icon-tile" link="/services"></MenuEntry>
			<template v-if="$authentication.isAuthenticated">
				<MenuEntry text="Add" icon="bzd-icon-add" link="/config"></MenuEntry>
				<MenuEntry text="Logout" icon="bzd-icon-close" @click="handleLogout"></MenuEntry>
			</template>
			<template v-else>
				<MenuEntry text="Login" icon="bzd-icon-user" link="/login"></MenuEntry>
			</template>
		</template>
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
	</Layout>
</template>

<script>
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import Layout from "#bzd/nodejs/vue/components/layout/layout.vue";
	import Tree from "./tree.vue";
	import MenuEntry from "#bzd/nodejs/vue/components/menu/entry.vue";

	export default {
		components: {
			Layout,
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
					{ path: "/config/{volume}", authentication: true, component: () => import("./config.vue") },
					{ path: "/config", authentication: true, component: () => import("./config.vue") },
					{ path: "/view/{path:*}", component: () => import("./view.vue") },
					{ path: "/services", component: () => import("./services.vue") },
					{
						path: "/login",
						handler: () => {
							this.$api.invoke("login", "artifacts");
						},
					},
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
			async handleLogout() {
				await this.$api.logout();
				this.$router.dispatch("/");
			},
			handleHeader() {
				this.$router.dispatch("/");
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/styles/default/css/base.scss" as *;
	@use "#bzd/nodejs/styles/default/css/tooltip.scss" as *;
	@use "#bzd/nodejs/styles/default/css/loading.scss" as *;

	@use "#bzd/nodejs/icons.scss" as icons with (
		$bzdIconNames: add tile user close
	);

	.bzd-icon-artifact {
		@include icons.defineIcon("svg/artifact.svg");
	}

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
