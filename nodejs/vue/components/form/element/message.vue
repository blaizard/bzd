<template>
	<p :class="containerClass" v-text="getOption('value', '')"></p>
</template>

<script>
	import Element from "./element.vue";

	export default {
		mixins: [Element],
		computed: {
			containerClass() {
				return {
					"irform-message": true,
					[this.getOption("class")]: true,
				};
			},
		},
	};
</script>
