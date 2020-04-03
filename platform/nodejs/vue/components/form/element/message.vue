<template>
	<p class="irform-message" v-text="getOption('value', '')"></p>
</template>

<script>
	"use strict";

	import Element from "./element.vue"

	export default {
		mixins: [Element]
	}
</script>
