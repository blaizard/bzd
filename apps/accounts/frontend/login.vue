<template>
	<Authentication title="LOGIN">
		<template v-if="!$authentication.isAuthenticated">
			<Form v-model="info" :description="formLoginDescription" @submit="handleSubmitLogin"></Form>
			<div class="reset">
				<RouterLink link="/reset">{{ $lang.getCapitalized("passwordforgot") }}</RouterLink>
			</div>
			<div class="or">or</div>
			<Form :description="formLoginTrustedDescription"></Form>
		</template>
		<template v-else>
			<span class="logged-in-message">Already logged-in</span>
		</template>
	</Authentication>
</template>

<script>
	import Authentication from "#bzd/apps/accounts/frontend/authentication_base.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
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
				async handler(isAuthenticated) {
					// Do this only if loading is not active to avoid a race when login.
					if (isAuthenticated && !this.loading) {
						await this.afterLogin();
					}
				},
				immediate: true,
			},
		},
		computed: {
			application() {
				return typeof URLSearchParams !== "undefined"
					? new URLSearchParams(window.location.search).get("application")
					: null;
			},
			formLoginTrustedDescription() {
				return [
					{
						type: "Button",
						content: "Login with Google",
						fill: true,
						click: async () => {
							await this.handleSubmit(async () => {
								await this.$api.invoke("google-authenticate");
								await this.afterLogin();
							});
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
			async afterLogin() {
				if (this.application) {
					const data = await this.$api.request("get", "/sso", { application: this.application });
					window.location.href = data.redirect + "?sso_token=" + encodeURIComponent(data.token);
				} else if (this.redirect) {
					window.location.href = this.redirect;
				} else {
					this.$router.dispatch("/");
				}
			},
			async handleSubmitLogin() {
				await this.handleSubmit(async () => {
					await this.$api.login(this.info.uid, this.info.password, this.info.persistent, "accounts");
					await this.afterLogin();
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
	.logged-in-message {
		display: block;
		text-align: center;
	}
</style>
