<template>
	<Authentication title="LOGOUT">
		<Button
			content="Logout"
			:description="descriptionLogoutButton"
			action="danger"
			@click="handleSubmitLogout"
		></Button>
	</Authentication>
</template>

<script>
	import Authentication from "#bzd/apps/accounts/frontend/authentication_base.vue";
	import Button from "#bzd/nodejs/vue/components/form/element/button.vue";
	import Base from "#bzd/apps/accounts/frontend/base.vue";

	export default {
		mixins: [Base],
		components: {
			Authentication,
			Button,
		},
		computed: {
			descriptionLogoutButton() {
				return {
					fill: true,
				};
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
