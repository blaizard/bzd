<template>
	<div :class="{'irform-item': true, 'irform-error': isError, 'irform-disable': disable}" :style="style">
		<div class="irform-caption" v-if="caption === ''">&nbsp;</div>
		<div class="irform-caption" v-else-if="caption !== null">{{ caption }}</div>
		<div :class="elementsClass">
			<slot :setError="setError"></slot>
			<span v-if="isError" class="irform-error-message"><i class="irform-icon-warning"></i> {{ errorList.join(" ") }}</span>
		</div>
	</div>
</template>

<script>
	"use strict";

	export default {
		props: {
			align: {type: String, required: false, default: "top left"},
			caption: {type: String | Object, required: false, default: null},
			disable: {type: Boolean, default: false, required: false},
			width: {type: Number | String, default: 1, required: false}
		},
		data: function() {
			return {
				errorList: []
			}
		},
		computed: {
			alignList() {
				return this.align.replace(/[^a-z]/i, " ").split(" ").map((align) => align.toLowerCase().trim());
			},
			elementsClass() {
				return {
					'irform-elements': true,
					'irform-align-top': (this.alignList.indexOf("top") != -1),
					'irform-align-bottom': (this.alignList.indexOf("bottom") != -1),
					'irform-align-left': (this.alignList.indexOf("left") != -1),
					'irform-align-right': (this.alignList.indexOf("right") != -1)
				}
			},
			isError() {
				return this.errorList.length > 0;
			},
			style() {
				if ((this.width || 1) === 1) {
					return {
						width: "100%"
					}
				}
				if (typeof this.width === "number") {
					return {
						flex: (this.width * 100) + " 0 0"
					};
				}
				return this.width;
			},
		},
		methods: {
			setError(arg) {
				if (arg instanceof Array) {
					this.errorList = arg;
				}
				else if (typeof arg === "string") {
					this.errorList = [arg];
				}
				else {
					this.errorList = [];
				}
			}
		}
	}
</script>
