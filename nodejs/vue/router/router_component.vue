<template>
	<component
		:is="component"
		v-bind="Object.assign({}, $attrs, additionalProps)"
		:data-bzd-router-id="routerId"
	></component>
</template>

<script>
	import Loading from "./router_loading.vue";
	import { markRaw, defineAsyncComponent } from "vue";

	export default {
		data: function () {
			return {
				component: null,
				routerId: null,
				additionalProps: {},
			};
		},
		emits: ["updated"],
		methods: {
			componentSet(component, routerId, props) {
				this.component =
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
				this.routerId = routerId;
				this.additionalProps = props;
				this.$emit("updated");
			},
		},
	};
</script>
