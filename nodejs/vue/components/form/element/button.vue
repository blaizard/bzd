<template>
	<button
		:class="buttonClass"
		@focus="setActive()"
		@blur="setInactive()"
		@click="handleClick()"
		:disabled="disable"
		:tabindex="disable ? undefined : 0"
		v-html="contentHtml"
	></button>
</template>

<script>
	import Element from "./element.vue";

	export default {
		mixins: [Element],
		props: {
			action: { type: String, optional: true, default: "" },
			content: { type: String || Object, optional: true, default: "" },
			width: { type: Number, optional: true, default: 0 },
		},
		emits: ["click"],
		computed: {
			/// ---- CONFIG ----------------------------------------
			click() {
				return this.getOption("click", null);
			},
			/// ---- IMPLEMENTATION ----------------------------------
			attrAction() {
				if (this.disable) {
					return {};
				}
				const actionToClass = {
					approve: {
						defaultClick: this.submit,
						class: "irform-button-approve",
					},
					cancel: {
						class: "irform-button-cancel",
					},
					danger: {
						defaultClick: this.submit,
						class: "irform-button-danger",
					},
				};
				return actionToClass[this.action || this.getOption("action")] || {};
			},
			// The content of the button
			buttonClass() {
				return {
					"irform-button": true,
					"irform-fill": this.getOption("fill", false),
					[this.attrAction.class || "nop"]: Boolean(this.attrAction.class || false),
					[this.getOption("class")]: true,
				};
			},
			// The content of the button
			attrContent() {
				return this.content || this.getOption("content", "Submit");
			},
			html() {
				return this.getContentType(this.attrContent) == "html";
			},
			contentHtml() {
				const data = this.getContentData(this.attrContent);
				return this.html ? data : this.toHtmlEntities(data);
			},
		},
		methods: {
			async handleClick() {
				this.$emit("click", this.context);
				await (this.click || this.attrAction.defaultClick || (() => {}))(this.context);
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/styles/default/css/form/button.scss" as *;
</style>
