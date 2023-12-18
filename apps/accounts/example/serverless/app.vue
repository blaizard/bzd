<template>
	<button @click="onClickLogout" v-if="authentication.isAuthenticated">Logout</button>
	<button @click="onClickLogin" v-else>Login</button>
	<div>Authenticated: {{ authentication.isAuthenticated }}</div>
	<div>User: {{ user }}</div>
</template>

<script setup>
	import { inject, watch, ref } from "vue";

	const api = inject("$api");
	const authentication = inject("$authentication");

	const onClickLogin = () => {
		api.invoke("login", "localhost");
	};

	const onClickLogout = async () => {
		await api.logout();
	};

	// Fetch user data when authenticated.
	const user = ref({});
	watch(
		() => authentication.isAuthenticated,
		async () => {
			user.value = await api.request("get", "/user");
		},
	);
</script>
