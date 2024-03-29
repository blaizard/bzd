<template>
	<div :class="{ 'irform-item': true, 'irform-error': isError, 'irform-disable': disable }" :style="style">
		<div class="irform-caption" v-if="caption === ''">&nbsp;</div>
		<div class="irform-caption" v-else-if="caption !== null">{{ caption }}<span v-if="mandatory">*</span></div>
		<div :class="elementsClass">
			<slot></slot>
			<span v-if="isError" class="irform-error-message"
				><i class="irform-icon-warning"></i> {{ error.join(", ") }}</span
			>
		</div>
	</div>
</template>

<script>
	export default {
		props: {
			align: { type: String, required: false, default: "top left" },
			caption: { type: [String, Object], required: false, default: null },
			disable: { type: Boolean, default: false, required: false },
			height: { validator: (v) => ["small", "normal", "large"].includes(v), default: "normal", required: false },
			/// Set the width of the element.
			/// A number from 0 to 1.
			/// - 1 being 100% of the width,
			/// - > 0 and  < 1, elements will be inlined with a ratio equal to the number.
			/// - 0, elements are inlined.
			/// A string sets a fixed width for the element (eg. 75%, 100px, ...), but elements are not inlined.
			width: { type: [Number, String], default: 1, required: false },
			mandatory: { type: Boolean, default: false, required: false },
			error: { type: Array, default: [], required: false },
		},
		data: function () {
			return {};
		},
		computed: {
			alignList() {
				return this.align
					.replace(/[^a-z]/i, " ")
					.split(" ")
					.map((align) => align.toLowerCase().trim());
			},
			elementsClass() {
				return {
					"irform-elements": true,
					"irform-align-top": this.alignList.indexOf("top") != -1,
					"irform-align-bottom": this.alignList.indexOf("bottom") != -1,
					"irform-align-left": this.alignList.indexOf("left") != -1,
					"irform-align-right": this.alignList.indexOf("right") != -1,
					"irform-align-center": this.alignList.indexOf("center") != -1,
					"irform-height-small": this.height == "small",
					"irform-height-normal": this.height == "normal",
					"irform-height-large": this.height == "large",
				};
			},
			isError() {
				return this.error.length > 0;
			},
			style() {
				if ((this.width || 1) === 1) {
					return {
						width: "100%",
					};
				}
				if (typeof this.width === "number") {
					return {
						flex: this.width * 100 + " 0 0",
					};
				}
				return this.width;
			},
		},
	};
</script>
