<template>
	<div class="bzd-login">
		<div class="logo">
			<i class="bzd-icon-logo"></i>
		</div>
		<div class="login" v-loading="loading">
			<h1>LOGIN</h1>
			<div v-if="error" class="error">{{ error }}</div>
			<Form v-model="info" :description="formLoginDescription" @submit="handleSubmit"></Form>
		</div>
		<!--<div class="signup">Not registered yet? <RouterLink link="/signup">Sign-up here</RouterLink></div>//-->
		<div class="forgot">
			<RouterLink link="/forgot">{{ $lang.getCapitalized("passwordforgot") }}</RouterLink>
		</div>
		<Button content="Login" @click="handleSubmit"></Button>
	</div>
</template>

<script>
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import Button from "#bzd/nodejs/vue/components/form/element/button.vue";

	export default {
		components: {
			Form,
			Button,
		},
		data: function () {
			return {
				info: {},
			};
		},
		computed: {
			formLoginDescription() {
				return [
					{
						type: "Input",
						name: "email",
						placeholder: this.$lang.getCapitalized("email"),
						pre: { html: '<i class="bzd-icon-email"></i>' },
						validation: "email",
					},
					{
						type: "Password",
						name: "password",
						placeholder: this.$lang.getCapitalized("password"),
						pre: { html: '<i class="bzd-icon-lock"></i>' },
					},
				];
			},
		},
		methods: {
			handleSubmit() {
				console.log("submit!", this.info);
			},
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
	.bzd-login {
		width: 500px;
		padding: 0 55px 37px;

		.logo {
			display: flex;
			justify-content: center;
			align-items: center;
			font-size: 80px;
		}

		.forgot {
			text-align: right;
		}
	}
</style>
