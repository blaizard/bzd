<template>
	<Layout :fullPage="true">
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

	export default {
		components: {
			Layout,
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
					{ path: "/", component: () => import("./admin/admin.vue"), authentication: true },
					{ path: "/login", component: () => import("./login.vue") },
					{ path: "/logout", component: () => import("./logout.vue") },
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

	@use "#bzd/nodejs/icons.scss" as icons with (
		$bzdIconNames: email lock
	);

	.bzd-icon-logo {
		@include icons.defineIcon("bzd.svg");
	}
</style>
