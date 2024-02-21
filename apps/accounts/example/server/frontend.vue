<template>
	<button @click="onClickLogout" v-if="authentication.isAuthenticated">Logout</button>
	<button @click="onClickLogin" v-else>Login</button>
	<div>Authenticated: {{ authentication.isAuthenticated }}</div>
</template>

<script setup>
	import { inject } from "vue";

	const rest = inject("$rest");
	const authentication = inject("$authentication");

	const onClickLogin = async () => {
		await rest.invoke("login", "localhost");
	};

	const onClickLogout = async () => {
		await rest.logout();
	};
</script>
