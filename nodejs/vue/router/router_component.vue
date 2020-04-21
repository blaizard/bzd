<template>
    <div>
        Left over path = {{ nestedPath }}
        <component :is="component" :bzd-router-prefix="nestedPath" :bzd-router-path="nestedPath" :data-bzd-router-id="routerId"></component>
    </div>
</template>

<script>
	"use strict"

	import Vue from "vue";

	export default {
		data: function () {
			return {
				component: null,
				nestedPath: null,
				routerId: null
			}
		},
	}
</script>
