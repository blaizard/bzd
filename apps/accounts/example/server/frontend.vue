<template>
	<button @click="onClickLogout" v-if="authentication.isAuthenticated">Logout</button>
	<button @click="onClickLogin" v-else>Login</button>
	<div>Authenticated: {{ authentication.isAuthenticated }}</div>
</template>

<script setup>
	import { inject } from "vue";

	const api = inject("$api");
	const authentication = inject("$authentication");

	const onClickLogin = () => {
		api.invoke("login", "localhost");
	};

	const onClickLogout = async () => {
		await api.logout();
	};
</script>
