<template>
	<div>
		<h1>Statistics</h1>
		<Statistics></Statistics>
	</div>
</template>

<script>
	import Statistics from "#bzd/nodejs/core/statistics/statistics.vue";

	export default {
		components: {
			Statistics,
		},
		data: function () {
			return {};
		},
	};
</script>
