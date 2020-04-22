<template>
    <component :is="component" :data-bzd-router-id="routerId"></component>
</template>

<script>
	"use strict"

	import Vue from "vue";

	export default {
		data: function () {
			return {
				component: null,
				routerId: null
			}
		},
		methods: {
			componentSet(component, routerId) {
				this.component = component;
				this.routerId = routerId;
			}
		}
	}
</script>
