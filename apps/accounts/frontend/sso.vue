<template>
	<Authentication title="SSO">
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
			description() {},
		},
		methods: {
			async handleSubmitLogout() {
				await this.handleSubmit(async () => {
					window.location.href = this.redirect + "?t=" + result.token + "&timeout=" + result.timeout;
				});
			},
		},
	};
</script>
