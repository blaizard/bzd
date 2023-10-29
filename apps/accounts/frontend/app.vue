<template>
	<Layout :fullPage="true">
		<template #header>
			<RouterLink link="/">Accounts</RouterLink>
		</template>
		<template #menu>
			<template v-if="$authentication.isAuthenticated">
				<MenuEntry text="Configuration" icon="bzd-icon-images" link="/"></MenuEntry>
				<MenuEntry text="Admin" icon="bzd-icon-newspaper" link="/admin"></MenuEntry>
			</template>
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
					{ path: "/admin", component: () => import("./admin/admin.vue"), authentication: true },
					{ path: "/login", component: () => import("./login.vue") },
					{ path: "/logout", component: () => import("./logout.vue"), authentication: true },
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
