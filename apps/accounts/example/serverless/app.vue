<template>
	<button @click="onClickLogout" v-if="authentication.isAuthenticated">Logout</button>
	<button @click="onClickLogin" v-else>Login</button>
	<div>Authenticated: {{ authentication.isAuthenticated }}</div>
	<div>User: {{ user }}</div>
</template>

<script setup>
	import { inject, watch, ref } from "vue";

	const rest = inject("$rest");
	const authentication = inject("$authentication");

	const onClickLogin = async () => {
		await rest.invoke("login", "localhost");
	};

	const onClickLogout = async () => {
		await rest.logout();
	};

	// Fetch user data when authenticated.
	const user = ref({});
	watch(
		() => authentication.isAuthenticated,
		async () => {
			user.value = await rest.request("get", "/user");
		},
	);
</script>
