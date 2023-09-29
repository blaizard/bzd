<template>
	<Authentication title="SIGNUP">
		<Form v-model="info" :description="formRegisterDescription" @submit="handleSubmit"></Form>
	</Authentication>
</template>

<script>
	import Authentication from "#bzd/apps/accounts/frontend/authentication_base.vue";
	import Form from "#bzd/nodejs/vue/components/form/form.vue";

	export default {
		components: {
			Authentication,
			Form,
		},
		data: function () {
			return {
				info: {},
			};
		},
		computed: {
			formRegisterDescription() {
				return [
					{
						type: "Input",
						name: "email",
						placeholder: this.$lang.getCapitalized("email"),
						pre: { html: '<i class="bzd-icon-email"></i>' },
						validation: "email mandatory",
					},
					{
						type: "Password",
						name: "password",
						placeholder: this.$lang.getCapitalized("password"),
						pre: { html: '<i class="bzd-icon-lock"></i>' },
						validation: "mandatory",
					},
					{
						type: "Button",
						action: "approve",
						content: "Create an account",
						fill: true,
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
