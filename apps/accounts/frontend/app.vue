<template>
	<Layout :fullPage="true">
		<template #header>
			<RouterLink link="/">Accounts</RouterLink>
		</template>
		<template #menu>
			<div v-if="$authentication.isAuthenticated">
				<MenuEntry text="Configuration" icon="bzd-icon-images" link="/"></MenuEntry>
				<MenuEntry text="Users" icon="bzd-icon-newspaper" :link="$routerFromPath('/admin/users')"></MenuEntry>
				<MenuEntry
					text="Applications"
					icon="bzd-icon-newspaper"
					:link="$routerFromPath('/admin/applications')"
				></MenuEntry>
				<MenuEntry text="Logout" icon="bzd-icon-newspaper" link="/logout"></MenuEntry>
			</div>
		</template>
		<template #content>
			<RouterComponent
				ref="view"
				@loading="handleLoading"
				@updated="handleUpdated"
				v-loading="loading"
			></RouterComponent>
		</template>
	</Layout>
</template>

<script>
	import Layout from "#bzd/nodejs/vue/components/layout/layout.vue";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.mjs";
	import MenuEntry from "#bzd/nodejs/vue/components/menu/entry.vue";
	import { scopeAdminUsers, scopeAdminApplications } from "#bzd/apps/accounts/backend/users/scopes.mjs";

	export default {
		components: {
			Layout,
			MenuEntry,
		},
		directives: {
			loading: DirectiveLoading,
		},
		data: function () {
			return {
				loading: false,
			};
		},
		mounted() {
			this.$routerSet({
				ref: "view",
				routes: [
					{ path: "/", component: () => import("./config.vue"), authentication: true },
					{ path: "/admin/users", component: () => import("./admin/users.vue"), authentication: scopeAdminUsers },
					{
						path: "/admin/applications",
						component: () => import("./admin/applications.vue"),
						authentication: scopeAdminApplications,
					},
					{ path: "/login", component: () => import("./login.vue") },
					{ path: "/logout", component: () => import("./logout.vue"), authentication: true },
					{ path: "/sso", component: () => import("./sso.vue"), authentication: true },
					{ path: "/reset", component: () => import("./reset.vue") },
					{ path: "/register", component: () => import("./register.vue") },
				],
				fallback: { component: () => import("./404.vue") },
			});
		},
		methods: {
			handleLoading(value) {
				this.loading = value;
			},
			handleUpdated() {
				this.loading = false;
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/styles/default/css/base.scss" as *;
	@use "#bzd/nodejs/styles/default/css/loading.scss" as *;
	@use "#bzd/nodejs/styles/default/css/tooltip.scss" as *;

	@use "#bzd/nodejs/icons.scss" as icons with (
		$bzdIconNames: email lock
	);

	.bzd-icon-logo {
		@include icons.defineIcon("bzd.svg");
	}
</style>
