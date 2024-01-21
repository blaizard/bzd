<template>
	<Authentication :title="title">
		<Form v-model="info" :description="formResetDescription" @submit="handleSubmitReset"></Form>
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
		props: {
			uid: { type: String, required: true },
			token: { type: String, required: true },
			first: { type: Boolean, required: false, default: false },
		},
		data: function () {
			return {
				info: {},
				sent: false,
			};
		},
		computed: {
			title() {
				return this.first ? "NEW PASSWORD" : "RESET";
			},
			formResetDescription() {
				return [
					{
						type: "Message",
						value:
							"Your password has been successfully " +
							(this.first ? "set" : "reset") +
							", you will now be redirected to the login page...",
						condition: () => this.sent,
					},
					{
						type: "Password",
						name: "password",
						height: "large",
						placeholder: this.$lang.getCapitalized("password"),
						pre: { html: '<i class="bzd-icon-lock"></i>' },
						validation: "mandatory",
						condition: () => !this.sent,
					},
					{
						type: "Password",
						height: "large",
						placeholder: this.$lang.getCapitalized("passwordconfirm"),
						pre: { html: '<i class="bzd-icon-lock"></i>' },
						validation: "mandatory same(password)",
						condition: () => !this.sent,
					},
					{
						type: "Button",
						action: "approve",
						content: this.first ? "Set Password" : "Reset Password",
						height: "large",
						fill: true,
						condition: () => !this.sent,
					},
				];
			},
		},
		methods: {
			async handleSubmitReset() {
				await this.handleSubmit(async () => {
					await this.$api.request("post", "/password-change", {
						password: this.info.password,
						uid: this.uid,
						token: this.token,
					});
					this.sent = true;
					setTimeout(() => {
						this.$router.dispatch("/login");
					}, 3000);
				});
			},
		},
	};
</script>
