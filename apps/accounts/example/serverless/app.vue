<template>
	<button @click="onClickLogin">Login!</button>
	<div>Authenticated: {{ authentication.isAuthenticated }}</div>
	<div>User: {{ user }}</div>
</template>

<script setup>
	import { onMounted, inject, watch, ref } from "vue";
	import Config from "#bzd/apps/accounts/example/serverless/config.json";

	const api = inject("$api");
	const authentication = inject("$authentication");

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

	// Fetch user data when authenticated.
	const user = ref({});
	watch(
		() => authentication.isAuthenticated,
		async () => {
			user.value = await api.request("get", "/user");
		},
	);
</script>
