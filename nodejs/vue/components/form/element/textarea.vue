<template>
	<component
		:class="containerClass"
		:is="tag"
		@input="handleInput"
		@focus="setActive()"
		@blur="setInactive()"
		@change="resize"
		@cut="resize"
		@paste="resize"
		@drop="resize"
		v-text="get(/*freezeOnEdit*/ true)"
		:disabled="disable"
		:placeholder="placeholder"
		:style="{ resize: autoResize ? 'none' : 'vertical' }"
	>
	</component>
</template>

<script>
	import Element from "./element.vue";
	export default {
		mixins: [Element],
		props: {
			value: { type: String, required: false, default: "" },
			tag: { type: String, required: false, default: "textarea" },
		},
		data: function () {
			return {
				autoResize: this.getOption("auto-resize", true),
				placeholder: this.getOption("placeholder", ""),
			};
		},
		computed: {
			containerClass() {
				return {
					"irform-textarea": true,
					[this.getOption("class")]: true,
				};
			},
			paddingTop() {
				return parseFloat(window.getComputedStyle(this.$el, null).getPropertyValue("padding-top"));
			},
		},
		mounted() {
			this.resize();
		},
		methods: {
			resize() {
				if (this.autoResize) {
					this.$el.style.height = "auto";
					this.$el.style.height = this.$el.scrollHeight + this.paddingTop + 2 + "px";
				}
			},
			handleInput(e) {
				this.set(e.target.value);
				this.resize();
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/styles/default/css/form/textarea.scss" as *;
</style>
