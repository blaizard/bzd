<template>
	<button v-if="getContentType(content) == 'text'"
			class="irform-button"
			@focus="setActive()"
			@blur="setInactive()"
			@click="click()"
			:disabled="disable"
			:tabindex="(disable) ? undefined : 0"
			v-text="getContentData(content)">
	</button>
	<button v-else-if="getContentType(content) == 'html'"
			class="irform-button"
			@focus="setActive()"
			@blur="setInactive()"
			@click="click()"
			:disabled="disable"
			:tabindex="(disable) ? undefined : 0"
			v-html="getContentData(content)">
	</button>
</template>

<script>
	"use strict";

	import Element from "./element.vue"

	export default {
		mixins: [Element],
		data: function() {
			return {
				/**
				 * Defines a callback to be triggered the submit button is called
				 */
				click: this.getOption("click", this.submit),
				/**
				 * The content of the button
				 */
				content: this.getOption("content", "Submit")
			}
		}
	}
</script>
