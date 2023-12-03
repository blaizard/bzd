<template>
	<button @click="onClickLogin">Login!</button>
	<div>Authenticated: {{ $authentication.isAuthenticated }}</div>
</template>

<script setup>
	import { onMounted } from "vue";

	const ACCOUNTS_URL = "http://127.0.0.1:8080";

	const onClickLogin = () => {
		window.location.href = ACCOUNTS_URL + "/login?application=localhost";
	};

	function setCookie(cname, cvalue, exdays) {
		const d = new Date();
		d.setTime(d.getTime() + exdays * 24 * 60 * 60 * 1000);
		let expires = "expires=" + d.toUTCString();
		document.cookie = cname + "=" + cvalue + ";" + expires + ";path=/;SameSite=None;Secure";
	}

	onMounted(() => {
		const maybeSSO = new URLSearchParams(window.location.search).get("t");
		if (maybeSSO) {
			setCookie("refresh_token", maybeSSO, 10);
			console.log("token", maybeSSO);
			//window.location.href = "/";
			console.log(document.cookie);
		}
	});
</script>
