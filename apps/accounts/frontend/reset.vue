<template>
	<Authentication title="RESET">
		<Form v-model="info" :description="formResetDescription" @submit="handleSubmit"></Form>
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
			formResetDescription() {
				return [
					{
						type: "Input",
						name: "email",
						placeholder: this.$lang.getCapitalized("email"),
						pre: { html: '<i class="bzd-icon-email"></i>' },
						validation: "email mandatory",
					},
					{
						type: "Button",
						action: "approve",
						content: "Reset Password",
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
