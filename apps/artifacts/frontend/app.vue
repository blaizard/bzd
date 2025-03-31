<template>
	<Layout :full-page="true">
		<template #header>
			<a @click="handleHeader"><i class="bzd-icon-artifact"></i> Artifacts</a>
		</template>
		<template #actions>
			<template v-if="$authentication.isAuthenticated">
				<MenuEntry text="Services" icon="bzd-icon-tile" link="/services"></MenuEntry>
				<MenuEntry v-if="$authentication.hasLogout()" text="Logout" icon="bzd-icon-close" link="/logout"></MenuEntry>
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
	import configGlobal from "#bzd/apps/artifacts/config.json" with { type: "json" };

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
					{ path: "/view/{path:*}", component: () => import("./view.vue") },
					{ path: "/services", component: () => import("./services.vue") },
					{
						path: "/login",
						handler: async () => {
							await this.$rest.invoke("login", configGlobal.application);
						},
					},
					{
						path: "/logout",
						handler: async () => {
							await this.$rest.logout();
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
				await this.$rest.logout();
				this.$router.dispatch("/");
			},
			handleHeader() {
				this.$router.dispatch("/");
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/styles/default/css/base.scss" as *;
	@use "@/nodejs/styles/default/css/tooltip.scss" as *;
	@use "@/nodejs/styles/default/css/loading.scss" as *;

	@use "@/nodejs/icons.scss" as icons with (
		$bzdIconNames: add tile user close user
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
