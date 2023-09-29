<template>
	<div class="bzd-authentication">
		<div class="logo">
			<i class="bzd-icon-logo"></i>
		</div>
		<div class="login">
			<h1>{{ title }}</h1>
			<div v-if="error" class="error">{{ error }}</div>
			<slot></slot>
		</div>
	</div>
</template>

<script>
	export default {
		props: {
			title: { mandatory: true, type: String },
			error: { mandatory: false, type: String, default: null },
		},
	};
</script>

<style lang="scss">
	.bzd-content {
		display: flex;
		flex-wrap: wrap;
		justify-content: center;
		align-items: center;
		padding: 15px;
		background-position: center;
		background-size: cover;
		background-repeat: no-repeat;
	}
</style>

<style lang="scss" scoped>
	.bzd-authentication {
		width: 500px;
		padding: 0 55px 37px;

		.logo {
			display: flex;
			justify-content: center;
			align-items: center;
			font-size: 80px;
		}
	}
</style>
