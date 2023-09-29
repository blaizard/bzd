<template>
	<div class="bzd-content">
		<RouterComponent ref="view"></RouterComponent>
		<div class="g-signin2" data-onsuccess="onSignIn"></div>
	</div>
</template>

<script>
	export default {
		methods: {},
		data: function () {
			return {};
		},
		mounted() {
			this.$routerSet({
				ref: "view",
				routes: [
					{ path: "/login", component: () => import("./login.vue") },
					{ path: "/reset", component: () => import("./reset.vue") },
					{ path: "/register", component: () => import("./register.vue") },
				],
			});
		},
		computed: {},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/styles/default/css/base.scss" as *;
	@use "#bzd/nodejs/icons.scss" as icons with (
		$bzdIconNames: email lock
	);

	.bzd-icon-logo {
		@include icons.defineIcon("bzd.svg");
	}

	.bzd-content {
		width: 100%;
		height: 100vh;
	}
</style>
