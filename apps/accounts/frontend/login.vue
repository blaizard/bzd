<template>
	<Authentication title="LOGIN">
		<Form v-model="info" :description="formLoginDescription" @submit="handleSubmitLogin"></Form>
		<div class="reset">
			<RouterLink link="/reset">{{ $lang.getCapitalized("passwordforgot") }}</RouterLink>
		</div>
		<div class="signup">
			<RouterLink link="/register">{{ $lang.getCapitalized("signup") }}</RouterLink>
		</div>
		<div class="or">or</div>
		<Form :description="formLoginTrustedDescription"></Form>
	</Authentication>
</template>

<script>
	import Authentication from "#bzd/apps/accounts/frontend/authentication_base.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import GoogleClient from "#bzd/nodejs/core/authentication/google/client.mjs";
	import Base from "#bzd/apps/accounts/frontend/base.vue";

	export default {
		mixins: [Base],
		components: {
			Authentication,
			Form,
		},
		data: function () {
			return {
				info: {},
				loggedIn: false,
			};
		},
		watch: {
			"$authentication.isAuthenticated": {
				handler(isAuthenticated) {
					// Do this only if loading is not active to avoid a race when login.
					if (isAuthenticated && !this.loading) {
						const route = this.$routerGet();
						this.$routerDispatch("/logout", route ? { query: { redirect: route } } : {});
					}
				},
				immediate: true,
			},
		},
		computed: {
			formLoginTrustedDescription() {
				return [
					{
						type: "Button",
						content: "Login with Google",
						fill: true,
						click: async () => {
							const client = new GoogleClient(
								"484561133642-u96s0aq8rlmvj80ca03udqeavcsfvln9.apps.googleusercontent.com",
							);
							await client.authenticate();
						},
					},
				];
			},
			formLoginDescription() {
				return [
					{
						type: "Input",
						name: "uid",
						height: "large",
						placeholder: this.$lang.getCapitalized("email"),
						pre: { html: '<i class="bzd-icon-email"></i>' },
						validation: "email mandatory",
					},
					{
						type: "Password",
						name: "password",
						height: "large",
						placeholder: this.$lang.getCapitalized("password"),
						pre: { html: '<i class="bzd-icon-lock"></i>' },
						validation: "mandatory",
					},
					{
						type: "Checkbox",
						name: "persistent",
						text: this.$lang.getCapitalized("persistent"),
					},
					{
						type: "Button",
						action: "approve",
						content: "Login",
						height: "large",
						fill: true,
					},
				];
			},
		},
		methods: {
			async handleSubmitLogin() {
				await this.handleSubmit(async () => {
					await this.$api.login(this.info.uid, this.info.password, this.info.persistent, "accounts");
					this.$routerDispatch(this.redirect || "/");
				});
			},
		},
	};
</script>

<style lang="scss" scoped>
	.reset,
	.signup {
		text-align: right;
	}
	.or {
		text-align: center;
	}
</style>
