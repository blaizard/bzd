<template>
	<Authentication title="RESET">
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
		data: function () {
			return {
				info: {},
				sent: false,
			};
		},
		computed: {
			formResetDescription() {
				return [
					{
						type: "Message",
						value:
							"If we found an eligible account associated with that email, we've sent password reset instructions to this email address.",
						condition: () => this.sent,
					},
					{
						type: "Input",
						name: "uid",
						placeholder: this.$lang.getCapitalized("email"),
						pre: { html: '<i class="bzd-icon-email"></i>' },
						validation: "email mandatory",
						height: "large",
						disable: this.sent,
					},
					{
						type: "Button",
						action: "approve",
						content: "Request Password Reset",
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
					await this.$api.request("post", "/password-reset", this.info);
					this.sent = true;
				});
			},
		},
	};
</script>
