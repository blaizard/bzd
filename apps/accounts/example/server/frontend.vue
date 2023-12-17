<template>
	<button @click="onClickLogout" v-if="authentication.isAuthenticated">Logout</button>
	<button @click="onClickLogin" v-else>Login</button>
	<div>Authenticated: {{ authentication.isAuthenticated }}</div>
</template>

<script setup>
	import { onMounted, inject, watch, ref } from "vue";
	import Config from "#bzd/apps/accounts/example/config.json";

	const api = inject("$api");
	const authentication = inject("$authentication");

	const onClickLogin = () => {
		window.location.href = Config.accounts + "/login?application=localhost";
	};

	const onClickLogout = async () => {
		await api.logout();
	};

	onMounted(async () => {
		const maybeRefreshToken = new URLSearchParams(window.location.search).get("sso_token");
		if (maybeRefreshToken) {
			window.history.pushState(null, null, "/");
			// POST this server
			await api.loginWithSSO(maybeRefreshToken);
		}
	});
</script>
