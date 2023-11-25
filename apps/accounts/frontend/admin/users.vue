<template>
	<div>
		<h1>Users</h1>
		<Users></Users>
	</div>
</template>

<script>
	import Users from "#bzd/apps/accounts/backend/users/users.vue";

	export default {
		components: {
			Users,
		},
		data: function () {
			return {};
		},
	};
</script>

<style lang="scss" scoped></style>
