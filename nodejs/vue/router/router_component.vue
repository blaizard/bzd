<template>
	<component
		:is="component"
		v-bind="Object.assign({}, $attrs, additionalProps)"
		v-on="$listeners"
		:data-bzd-router-id="routerId"
	></component>
</template>

<script>
	import Loading from "./router_loading.vue";

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
						? () => ({
								// The component to load (should be a Promise)
								component: component(),
								// A component to use while the async component is loading
								loading: Loading,
								//  A component to use if the load fails
								// error: PageError,
								//  Delay before showing the loading component. Default: 200ms.
								delay: 200,
								//  The error component will be displayed if a timeout is
								//  provided and exceeded. Default: Infinity.
								// timeout: 3000
						  })
						: component;
				this.routerId = routerId;
				this.additionalProps = props;
				this.$emit("updated");
			},
		},
	};
</script>
