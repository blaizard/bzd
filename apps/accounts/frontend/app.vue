<template>
	<Layout :fullPage="true">
		<template #header>
			<RouterLink link="/">Accounts</RouterLink>
		</template>
		<template #menu>
			<div v-if="$authentication.isAuthenticated">
				<MenuEntry text="Configuration" icon="bzd-icon-images" link="/"></MenuEntry>
				<MenuEntry text="Contact" icon="bzd-icon-images" link="/contact"></MenuEntry>
				<MenuEntry text="Users" icon="bzd-icon-newspaper" :link="$router.fromPath('/admin/users')"></MenuEntry>
				<MenuEntry
					text="Applications"
					icon="bzd-icon-newspaper"
					:link="$router.fromPath('/admin/applications')"
				></MenuEntry>
				<MenuEntry text="Services" icon="bzd-icon-newspaper" :link="$router.fromPath('/admin/services')"></MenuEntry>
				<MenuEntry text="Logs" icon="bzd-icon-newspaper" :link="$router.fromPath('/admin/logs')"></MenuEntry>
				<MenuEntry text="Logout" icon="bzd-icon-newspaper" link="/logout"></MenuEntry>
			</div>
		</template>
		<template #content>
			<RouterComponent
				name="view"
				@loading="handleLoading"
				@updated="handleUpdated"
				v-loading="loading"
				class="bzd-accounts-content"
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
			this.$router.set({
				component: "view",
				routes: [
					{ path: "/", component: () => import("./config.vue"), authentication: true },
					{ path: "/admin/users", component: () => import("./admin/users.vue"), authentication: scopeAdminUsers },
					{
						path: "/admin/applications",
						component: () => import("./admin/applications.vue"),
						authentication: scopeAdminApplications,
					},
					{
						path: "/admin/services",
						component: () => import("./admin/services.vue"),
						authentication: scopeAdminApplications,
					},
					{
						path: "/admin/logs",
						component: () => import("./admin/logs.vue"),
						authentication: scopeAdminApplications,
					},
					{ path: "/privacy", component: () => import("./privacy.vue") },
					{ path: "/login", component: () => import("./login.vue") },
					{ path: "/logout", component: () => import("./logout.vue"), authentication: true },
					{ path: "/sso", component: () => import("./sso.vue"), authentication: true },
					{ path: "/reset/{uid}/{token}", component: () => import("./password_change.vue") },
					{ path: "/new/{uid}/{token}", component: () => import("./password_change.vue"), props: { first: true } },
					{ path: "/reset", component: () => import("./password_reset.vue") },
					{ path: "/contact", component: () => import("./contact.vue") },
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
	@use "@/nodejs/styles/default/css/base.scss" as *;
	@use "@/nodejs/styles/default/css/loading.scss" as *;
	@use "@/nodejs/styles/default/css/tooltip.scss" as *;

	@use "@/nodejs/icons.scss" as icons with (
		$bzdIconNames: email lock
	);

	.bzd-icon-logo {
		@include icons.defineIcon("bzd.svg");
	}
</style>

<style lang="scss" scoped>
	.bzd-accounts-content {
		padding: 20px;
	}
</style>
