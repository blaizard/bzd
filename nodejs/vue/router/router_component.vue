<template>
	<component :is="component" v-bind="$attrs" :data-bzd-router-id="routerId"></component>
</template>

<script>
"use strict";

export default {
	data: function () {
		return {
			component: null,
			routerId: null
		};
	},
	methods: {
		componentSet(component, routerId) {
			this.component = component;
			this.routerId = routerId;
		}
	}
};
</script>
