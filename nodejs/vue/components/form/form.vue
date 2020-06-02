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
					:active="active == index"
					:mandatory="isMandatory(index)"
					:error="getError(current, index)">
				<template>
					<component :is="getType(current)"
							v-model="currentValue[indexToName[index]]"
							@error="handleError(indexToName[index], $event)"
							@active="handleActive(index, $event)"
							@submit="handleSubmit(current)"
							@input="handleInput(index, $event)"
							:description="current"
							:disable="getDisable(current)">
					</component>
				</template>
			</component>

			<div v-if="isLineBreakNeeded(current, index)" class="irform-linebreak"></div>
		</template>
		<!--Value: {{ returnedValue }}//-->
	</component>
</template>

<script>
"use strict";

import Item from "./item.vue";
import Validation from "../../../core/validation.mjs";

// Include all the supported elements
import ElementInput from "./element/input.vue";
import ElementPassword from "./element/password.vue";
import ElementCheckbox from "./element/checkbox.vue";
import ElementTextarea from "./element/textarea.vue";
import ElementDropdown from "./element/dropdown.vue";
import ElementAutocomplete from "./element/autocomplete.vue";
import ElementButton from "./element/button.vue";
import ElementArray from "./element/array.vue";
import ElementTable from "./element/table.vue";
import ElementFile from "./element/file.vue";
import ElementCarousel from "./element/carousel.vue";
import ElementEditor from "./element/editor.vue";
import ElementMessage from "./element/message.vue";
import ElementDate from "./element/date.vue";
import ElementUnknown from "./element/unknown.vue";
const elements = {
	Input: ElementInput,
	Password: ElementPassword,
	Checkbox: ElementCheckbox,
	Texarea: ElementTextarea,
	Dropdown: ElementDropdown,
	Autocomplete: ElementAutocomplete,
	Button: ElementButton,
	Array: ElementArray,
	Table: ElementTable,
	File: ElementFile,
	Carousel: ElementCarousel,
	Editor: ElementEditor,
	Message: ElementMessage,
	Date: ElementDate
};

export default {
	/**
	 * Rename the elements to create custom element names to ensure no conflicts with elements:
	 * { InputFormElement: Input, ... }
	 */
	components: Object.assign({ ElementUnknown }, Object.keys(elements).reduce((obj, key) => {
		obj[key + "FormElement"] = elements[key];
		return obj;
	}, {})),
	props: {
		description: { type: Array, required: true },
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
			/*
			 * Those 2 values are used so that only the update is sent
			 * back to the input event
			 */
			currentValue: Object.assign({}, this.value),
			updatedValue: {},
			active: -1,
			errors: {}
		};
	},
	computed: {
		returnedValue() {
			return (this.diff) ? this.updatedValue : this.currentValue;
		},
		isError() {
			return Object.keys(this.errors).length > 0;
		},
		validationSchema() {
			let validation = {};
			for (const index in this.description) {
				const current = this.description[index];
				if ("validation" in current) {
					const name = this.indexToName[index];
					validation[name] = current.validation;
				}
			}
			return validation;
		},
		validation() {
			return new Validation(this.validationSchema);
		},
		indexToName() {
			return this.description.map((description, index) => this.getName(description, index));
		}
	},
	watch: {
		value() {
			this.currentValue = Object.assign({}, this.value);
		}
	},
	methods: {
		getError(description, index) {
			const name = this.indexToName[index];
			return (name in this.errors) ? this.errors[name] : [];
		},
		handleInput(index, value) {
			const name = this.indexToName[index];
			this.$set(this.updatedValue, name, value);
			this.$emit("input", this.returnedValue);
		},
		handleSubmit(/*description*/) {

			const result = this.validation.validate(this.currentValue, {
				output: "return",
				valueExists: (key, value) => {
					return (value !== undefined) && (value !== "");
				}
			});
			for (const name in result) {
				this.handleError(name, result[name]);
			}

			if (!this.isError) {
				this.$emit("submit", this.returnedValue);
			}
		},
		handleActive(index, data) {
			this.active = (typeof data === "object" && "id" in data) ? index : -1;
			this.$emit("active", data);
		},
		handleError(name, messageList) {
			if (messageList) {
				this.$set(this.errors, name, messageList);
			}
			else {
				this.$delete(this.errors, name);
			}
		},
		isMandatory(index) {
			const name = this.indexToName[index];
			return this.validation.isMandatory(name);
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
				return (description.type in elements) ? (description.type + "FormElement") : "Unknown";
			}
			return description.type;
		},
		getName(description, index) {
			let name = description.name || ("key-" + index);
			if (!(name in this.currentValue)) {
				this.$set(this.currentValue, name, undefined);
			}
			return name;
		},
		getDisable(description) {
			return ("disable" in description) ? Boolean(description.disable) : this.disable;
		},
		isConditionSatisfied(description) {
			if (typeof description.condition === "function") {
				return description.condition(this.currentValue);
			}
			return (typeof description.condition === "undefined") ? true : description.condition;
		}
	}
};
</script>

<style lang="scss">
	@use "bzd-style/css/form/layout.scss";
</style>
