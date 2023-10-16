<template>
	<Authentication title="LOGOUT">
		<Form :description="description"></Form>
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
		computed: {
			description() {
				return [
					{
						type: "Button",
						content: "Logout",
						action: "danger",
						fill: true,
						height: "large",
						click: this.handleSubmitLogout,
					},
				];
			},
		},
		methods: {
			async handleSubmitLogout() {
				await this.handleSubmit(async () => {
					await this.$api.logout();
					this.$routerDispatch(this.redirect || "/");
				});
			},
		},
	};
</script>
