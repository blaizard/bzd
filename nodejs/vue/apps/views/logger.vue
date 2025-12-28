<template>
	<div>
		<h1>Logs</h1>
		<Logger></Logger>
	</div>
</template>

<script>
	import Logger from "#bzd/nodejs/vue/components/logger/frontend/logger.vue";

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
