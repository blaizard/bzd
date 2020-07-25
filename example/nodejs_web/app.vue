<template>
	<p>
		Hello:
		<button @click="increment">Count is: {{ count }}, double is {{ double }}, click to increment.</button>
	</p>
</template>

<script>
	import { ref, computed, onMounted } from "@vue/composition-api";

	export default {
		setup() {
			const count = ref(0);
			const double = computed(() => count.value * 2);

			function increment() {
				count.value++;
			}

			onMounted(() => console.log("component mounted!"));

			return {
				count,
				double,
				increment,
			};
		},
	};
</script>
