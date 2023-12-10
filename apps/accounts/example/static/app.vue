<template>
	<button @click="onClickLogin">Login!</button>
	<div>Authenticated: {{ $authentication.isAuthenticated }}</div>
</template>

<script setup>
	import { onMounted, inject } from "vue";
	import Config from "#bzd/apps/accounts/example/static/config.json";

	const api = inject("$api");

	const onClickLogin = () => {
		window.location.href = Config.accounts + "/login?application=localhost";
	};

	onMounted(async () => {
		const maybeRefreshToken = new URLSearchParams(window.location.search).get("sso_token");
		if (maybeRefreshToken) {
			window.history.pushState(null, null, "/");
			await api.loginWithSSO(maybeRefreshToken);
		}
	});
</script>
