<template>
	<component class="irform" :is="tag">

		<template v-for="(current, index) in description">
			<component v-if="isConditionSatisfied(current)"
					:key="index"
					:is="template"
					:caption="current.caption"
					:align="current.align"
					:disable="getDisable(current)"
					:width="current.width"
					:active="active == index">

				<component slot-scope="item"
						:is="getType(current)"
						v-model="currentValue[getName(current, index)]"
						@error="item.setError($event)"
						@active="handleActive(index, $event)"
						@submit="handleSubmit(current)"
						@input="handleInput(getName(current, index), $event)"
						:description="current"
						:disable="getDisable(current)">
				</component>
			</component>

			<div v-if="isLineBreakNeeded(current, index)" class="irform-linebreak"></div>
		</template>
		<!--Value: {{ returnedValue }}//-->
	</component>
</template>

<script>
	"use strict";

	import Item from "./item.vue"

	// Include all the supported elements
	import Input from "./element/input.vue"
	import Checkbox from "./element/checkbox.vue"
	import Textarea from "./element/textarea.vue"
	import Dropdown from "./element/dropdown.vue"
	import Autocomplete from "./element/autocomplete.vue"
	import Submit from "./element/submit.vue"
	import Array from "./element/array.vue"
	import Table from "./element/table.vue"
	import File from "./element/file.vue"
	import Carousel from "./element/carousel.vue"
	import Editor from "./element/editor.vue"
	import Message from "./element/message.vue"
	import Unknown from "./element/unknown.vue"
	const elements = {Input, Checkbox, Textarea, Dropdown, Autocomplete, Submit, Array, Table, File, Carousel, Editor, Message};

	export default {
		/**
		 * Rename the elements to create cursomt element names to ensure no conflicts with elements:
		 * { InputFormElement: Input, ... }
		 */
		components: Object.assign({ Unknown }, Object.keys(elements).reduce((obj, key) => {
			obj[key + "FormElement"] = elements[key];
			return obj;
		}, {})),
		props: {
			description: { type: Object | Array, required: true },
			tag: { type: String, default: "div", required: false },
			template: { type: Object, default: () => Item, required: false },
			value: { type: Object, default: () => {}, required: false },
			disable: { type: Boolean, default: false, required: false },
			/**
			 * Only the diff will be returned
			 */
			diff: { type: Boolean, default: false, required: false }
		},
		name: "FormElement",
		data: function() {
			return {
				// Those 2 values are used so that only the update is sent
				// back to the input event
				currentValue: Object.assign({}, this.value),
				updatedValue: {},
				active: -1
			}
		},
		computed: {
			returnedValue() {
				return (this.diff) ? this.updatedValue : this.currentValue;
			}
		},
		watch: {
			value() {
				this.currentValue = Object.assign({}, this.value);
			}
		},
		methods: {
			handleInput(id, value) {
				this.$set(this.updatedValue, id, value);
				this.$emit("input", this.returnedValue);
			},
			handleSubmit(description) {
				this.$emit("submit", this.returnedValue);
			},
			handleActive(index, data) {
				this.active = (typeof data === "object" && data.hasOwnProperty("id")) ? index : -1;
				this.$emit("active", data);
			},
			isLineBreakNeeded(description, index) {
				if ((description.width || 1) === 1) {
					return false;
				}
				let lineWidth = 0;
				for (let i=0; i<=index; ++i) {
					lineWidth = ((lineWidth >= 1) ? 0 : lineWidth) + (this.description[i].width || 1);
				}
				return (lineWidth >= 1);
			},
			getType(description) {
				if (typeof description.type === "string") {
					return (elements.hasOwnProperty(description.type)) ? (description.type + "FormElement") : "Unknown";
				}
				return description.type;
			},
			getName(description, index) {
				let name = description.name || ("key-" + index);
				if (!this.currentValue.hasOwnProperty(name))
				{
					this.$set(this.currentValue, name, undefined);
				}
				return name;
			},
			getDisable(description) {
				return (description.hasOwnProperty("disable")) ? Boolean(description.disable) : this.disable;
			},
			isConditionSatisfied(description) {
				if (typeof description.condition === "function") {
					return description.condition(this.currentValue);
				}
				return (typeof description.condition === "undefined") ? true : description.condition;
			}
		}
	}
</script>
