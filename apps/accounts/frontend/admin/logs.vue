<template>
	<div>
		<h1>Logs</h1>
		<Logger></Logger>
	</div>
</template>

<script>
	import Logger from "#bzd/apps/accounts/backend/logger/memory/memory.vue";

	export default {
		components: {
			Logger,
		},
		data: function () {
			return {};
		},
	};
</script>

<style lang="scss" scoped></style>
