<template>
	<button :class="buttonClass"
			@focus="setActive()"
			@blur="setInactive()"
			@click="handleClick()"
			:disabled="disable"
			:tabindex="(disable) ? undefined : 0"
			v-html="contentHtml">
	</button>
</template>

<script>
"use strict";

import Element from "./element.vue";

export default {
	mixins: [Element],
	props: {
		action: {type: String, optional: true, default: ""},
		content: {type: String || Object, optional: true, default: ""}
	},
	data: function() {
		return {
			/**
						* Defines a callback to be triggered the submit button is called
						*/
			click: this.getOption("click", this.submit),
		};
	},
	computed: {
		attrAction() {
			const actionToClass = {
				"approve": {
					defaultClick: this.submit,
					class: "irform-button-approve"
				},
				"cancel": {
					class: "irform-button-cancel"
				},
				"danger": {
					defaultClick: this.submit,
					class: "irform-button-danger"
				}
			};
			return actionToClass[this.action || this.getOption("action")] || {};
		},
		/**
					* The content of the button
					*/
		buttonClass() {
			return {
				"irform-button": true,
				[this.attrAction.class || "nop"]: Boolean(this.attrAction.class || false)
			};
		},
		/**
					* The content of the button
					*/
		attrContent() {
			return this.content || this.getOption("content", "Submit");
		},
		html() {
			return (this.getContentType(this.attrContent) == "html");
		},
		contentHtml() {
			const data = this.getContentData(this.attrContent);
			return (this.html) ? data : this.toHtmlEntities(data);
		}
	},
	methods: {
		async handleClick() {
			this.$emit("click");
			await (this.attrAction.defaultClick || (() => {}))();
		}
	}
};
</script>

<style lang="scss">
	@use "bzd-style/css/form/button.scss";
</style>
