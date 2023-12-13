<template>
	<component :is="context.component" v-bind="Object.assign({}, $attrs, context.additionalProps)"></component>
</template>

<script setup>
	import Loading from "./router_loading.vue";
	import { markRaw, defineAsyncComponent, inject, onMounted, onBeforeUnmount, reactive } from "vue";

	const props = defineProps({
		name: {
			type: String,
			required: true,
		},
	});
	const emit = defineEmits(["updated"]);

	const context = reactive({
		component: null,
		additionalProps: {},
	});

	const registry = inject("$routerRegistry");
	onMounted(() => {
		registry.register(props.name, (component, props) => {
			context.component =
				typeof component === "function"
					? // See: https://vuejs.org/guide/components/async.html
						markRaw(
							defineAsyncComponent({
								// The component to load (should be a Promise)
								loader: component,
								// A component to use while the async component is loading
								loadingComponent: Loading,
								//  A component to use if the load fails
								// error: PageError,
								//  Delay before showing the loading component. Default: 200ms.
								delay: 200,
								//  The error component will be displayed if a timeout is
								//  provided and exceeded. Default: Infinity.
								// timeout: 3000
							}),
						)
					: markRaw(component);
			context.additionalProps = props;
			emit("updated");
		});
	});
	onBeforeUnmount(() => {
		registry.unregister(props.name);
	});
</script>
