<template>
	<button @click="onClickLogin">Login!</button>
	<div>Authenticated: {{ $authentication.isAuthenticated }}</div>
</template>

<script setup>
	import { onMounted, inject } from "vue";

	const ACCOUNTS_URL = "http://localhost:8080";
	const api = inject("$api");

	const onClickLogin = () => {
		window.location.href = ACCOUNTS_URL + "/login?application=localhost";
	};

	onMounted(async () => {
		const maybeRefreshToken = new URLSearchParams(window.location.search).get("sso_token");
		if (maybeRefreshToken) {
			window.history.pushState(null, null, "/");
			await api.loginWithSSO(maybeRefreshToken);
		}
	});
</script>
