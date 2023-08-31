<template>
	<div>
		<div class="login" v-loading="loading">
			<h1>Login</h1>
			<div v-if="error" class="error">{{ error }}</div>
			<Form v-model="login" :description="formLoginDescription" @submit="handleSubmit"></Form>
		</div>
	</div>
</template>

<script>
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.mjs";

	export default {
		components: {
			Form,
		},
		directives: {
			loading: DirectiveLoading,
		},
		data: function () {
			return {
				login: {},
				loading: false,
				error: null,
			};
		},
		mounted() {
			if (this.redirect) {
				this.$notification.info("The page requested requires authentication, please login first");
			}
		},
		computed: {
			redirect() {
				return typeof URLSearchParams !== "undefined"
					? new URLSearchParams(window.location.search).get("redirect")
					: null;
			},
			formLoginDescription() {
				return this.$api.updateForm("post", "/auth/login", [
					{
						type: "Input",
						name: "uid",
						caption: "e-mail",
						validation: "mandatory",
						pre: { html: "<i class=\"bzd-icon-email\"></i>" },
					},
					{
						type: "Password",
						name: "password",
						validation: "mandatory",
						caption: "Password",
						pre: { html: "<i class=\"bzd-icon-lock\"></i>" },
					},
					{ type: "Checkbox", name: "persistent", caption: "", text: "Stay connected" },
					{ type: "Button", action: "approve", content: "Login", align: "right" },
				]);
			},
		},
		methods: {
			async handleSubmit() {
				this.error = null;
				this.loading = true;
				try {
					await this.$api.login(this.login.uid, this.login.password, this.login.persistent);
					this.$routerDispatch(this.redirect || "/");
				}
				catch (e) {
					this.error = "Wrong username or password";
				}
				finally {
					this.loading = false;
				}
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/icons.scss" as * with (
		$bzdIconNames: email lock
	);
</style>

<style lang="scss" scoped>
	@use "#bzd/nodejs/styles/default/css/colors.module.scss" as colors;

	.login {
		width: 300px;
		margin: 0 auto;

		.error {
			text-align: center;
			color: colors.$bzdGraphColorRed;
		}
	}

	.signup {
		margin-top: 20px;
		text-align: center;
	}
</style>
