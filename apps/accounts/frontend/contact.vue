<template>
	<div class="contact">
		<h1>Contact</h1>
		<Form v-model="info" :description="formContactDescription" @submit="handleSubmitContact"></Form>
	</div>
</template>

<script>
	import Form from "#bzd/nodejs/vue/components/form/form.vue";
	import Base from "#bzd/apps/accounts/frontend/base.vue";
	import Config from "#bzd/apps/accounts/config.json";

	export default {
		mixins: [Base],
		data: function () {
			return {
				sent: false,
				info: {},
			};
		},
		components: {
			Form,
		},
		computed: {
			formContactDescription() {
				return [
					{
						type: "Message",
						value:
							"Thank you for your message, it has been successfully sent. Our team will reply to you as soon as possible.",
						condition: () => this.sent,
					},
					{
						name: "email",
						type: "Input",
						caption: "Your e-mail",
						fill: true,
						validation: "mandatory email",
						condition: () => !this.sent && !this.$authentication.isAuthenticated,
					},
					{
						name: "subject",
						type: "Input",
						caption: "Subject",
						fill: true,
						validation: "mandatory",
						condition: () => !this.sent,
					},
					{
						name: "content",
						type: "Textarea",
						caption: "Message",
						fill: true,
						validation: "mandatory",
						condition: () => !this.sent,
					},
					{
						name: "captcha",
						type: "Button",
						action: "approve",
						content: "Send",
						height: "large",
						align: "right",
						googleCaptcha: Config.googleCaptchaSiteKey,
						condition: () => !this.sent,
					},
				];
			},
		},
		async mounted() {
			const urlParams = new URLSearchParams(window.location.search);
			this.info = {
				email: urlParams.get("email", ""),
				subject: urlParams.get("subject", ""),
				content: urlParams.get("content", ""),
			};
		},
		methods: {
			async handleSubmitContact() {
				await this.handleSubmit(async () => {
					if (this.$authentication.isAuthenticated) {
						await this.$api.request("post", "/contact-authenticated", this.info);
					} else {
						await this.$api.request("post", "/contact", this.info);
					}
					this.sent = true;
				});
			},
		},
	};
</script>

<style lang="scss" scoped>
	.contact {
		margin: 0 auto;
		max-width: 1024px;
	}
</style>
