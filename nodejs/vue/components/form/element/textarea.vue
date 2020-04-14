<template>
	<component class="irform-textarea"
			:is="tag"
			:value="get()"
			@input="handleInput"
			@focus="setActive()"
			@blur="setInactive()"
			@change="resize"
			@cut="resize"
			@paste="resize"
			@drop="resize"
			:disabled="disable"
			:placeholder="placeholder"
			:style="{resize: (autoResize) ? 'none' : 'vertical'}">
		<slot></slot>
	</component>
</template>

<script>
	"use strict";

	import Element from "./element.vue"
	export default {
		mixins: [Element],
		props: {
			value: {type: String, required: false, default: ""},
			tag: {type: String, required: false, default: "textarea"}
		},
		data: function() {
			return {
				autoResize: this.getOption("auto-resize", true),
				placeholder: this.getOption("placeholder", "")
			};
		},
		computed: {
			paddingTop() {
				return parseFloat(window.getComputedStyle(this.$el, null).getPropertyValue('padding-top'));
			}
		},
		mounted() {
			this.resize();
		},
		methods: {
			resize() {
				if (this.autoResize) {
					this.$el.style.height = "auto";
					this.$el.style.height = (this.$el.scrollHeight + this.paddingTop + 2) + "px";
				}
			},
			handleInput(e) {
				this.set(e.target.value);
				this.resize();
			}
		}
	}
</script>
