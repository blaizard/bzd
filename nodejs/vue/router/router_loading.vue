<template>
	<div v-loading="true">&nbsp;</div>
</template>

<script>
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.js";

	export default {
		directives: {
			loading: DirectiveLoading,
		},
	};
</script>
