<template>
	<Element name="irgraph-donut" :value="valueList" :selected="selected" v-slot:default="slotProps">
		<svg viewBox="0 0 100 100" v-hover-children="selected">
			<circle v-for="style, index in valueStyles"
					:class="getCircleClass(index, slotProps.selected, selected)"
					:key="index"
					r="25%" cx="50%" cy="50%"
					:style="style">
			</circle>
			<g v-if="text" class="irgraph-donut-center-text">
				<text x="50%" y="50%">{{ text }}</text>
			</g>
		</svg>
	</Element>
</template>

<script>
	"use strict";

	import Element from "./element.vue"
	import HoverChildren from "./directive/hover-children.js"

	export default {
		components: {
			Element
		},
		props: {
			value: {type: Array, required: false, default: () => []},
			text: {type: String | Number, required: false, default: false}
		},
		directives: {
			"hover-children": HoverChildren
		},
		data: function() {
			return {
				selected: -1
			}
		},
		computed: {
			valueList() {
				const value = (this.value instanceof Array) ? this.value : [];
				return value.map((item) => {
					let defaultItem = {
						caption: "",
						value: 0,
						color: "red"
					};
					if (typeof item === "object") {
						return Object.assign(defaultItem, item);
					}
					defaultItem.value = item;
					return defaultItem;
				});
			},
			valueStyles() {
				const circumference = 2 * Math.PI * 25;
				let circumferenceList = this.valueList.map((item) => {
					// Ceil is on purpose to make even small items visible, but ones with null value invisible
					return Math.ceil(circumference * item.value);
				});

				// If the sum of item values is equal to 1 (or 100%)
				if (Math.abs(1 - this.valueList.reduce((sum, item) => sum + item.value, 0)) < 0.01) {
					let diffCircumference = Math.round(circumferenceList.reduce((sum, n) => sum + n, 0) - circumference);
					// Need to adjust the circumferenceList by removing diffCircumference points
					const indexMax = circumferenceList.indexOf(Math.max(...circumferenceList));
					circumferenceList[indexMax] -= diffCircumference;
				}

				let curOffset = 0;
				return circumferenceList.map((circumferenceCovered) => {
					const style = {
						strokeDashoffset: -curOffset + "px",
						strokeDasharray: circumferenceCovered + " " + (circumference - circumferenceCovered)
					};
					curOffset += circumferenceCovered;
					return style;
				});
			}
		},
		methods: {
			getCircleClass(index, selected1, selected2) {
				return {
					"irgraph-selected": (index === ((selected1 === -1) ? selected2 : selected1))
				}
			}
		}
	}
</script>
