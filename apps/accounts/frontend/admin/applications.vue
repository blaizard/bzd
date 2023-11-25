<template>
	<div>
		<h1>Applications</h1>
		<Applications></Applications>
	</div>
</template>

<script>
	import Applications from "#bzd/apps/accounts/backend/applications/applications.vue";

	export default {
		components: {
			Applications,
		},
		data: function () {
			return {};
		},
	};
</script>

<style lang="scss" scoped></style>
