<template>
	<div>
		<h1>Config</h1>
		<User></User>
	</div>
</template>

<script>
	import User from "#bzd/apps/accounts/backend/users/user.vue";

	export default {
		components: {
			User,
		},
		data: function () {
			return {};
		},
	};
</script>

<style lang="scss" scoped></style>
