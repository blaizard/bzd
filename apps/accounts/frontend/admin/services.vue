<template>
	<div>
		<h1>Services</h1>
		<Services></Services>
	</div>
</template>

<script>
	import Services from "#bzd/nodejs/core/services/services.vue";

	export default {
		components: {
			Services,
		},
		data: function () {
			return {};
		},
	};
</script>

<style lang="scss" scoped></style>
