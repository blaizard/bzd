<script>
"use strict";

import Validation from "../../../../core/validation.mjs";

/**
 * The following must be implement for all elements:
 * Events:
 * - @active: when the element has the focus
 * - @input: when the element has a new value
 * - @error: when the element failed
 * Props:
 * - disable: set the element as disabled
 *
 * It handles the following keys from the "description" property:
 * - validation: syntax compatible with validation module.
 * - valueType: a string defining the type of data present. It can be "number", "list" or "any".
 */
export default {
	props: {
		description: {type: Object, required: false, default: () => ({})},
		disable: {type: Boolean, default: false, required: false}
	},
	data: function() {
		return {
			uid: this._uid,
			internalIsValueFrozen: false,
			internalFrozenValue: "",
			isActive: false
		};
	},
	computed: {
		valueType() {
			return this.getOption("valueType", "any");
		},
		externalValue() {
			switch (this.valueType) {
			case "list":
			{
				let list = this.value || [];
				if (!(list instanceof Array)) {
					this.setError("The value must be an Array");
					list = [];
				}
				return list;
			}
			case "number":
			case "any":
				break;
			default:
				this.setError("Unknown value type \"" + this.valueType + "\"");
			}

			return this.value;
		},
		internalValue() {
			if (this.internalIsValueFrozen) {
				return this.internalFrozenValue;
			}
			return this.externalValue;
		},
		validation() {
			if ("validation" in this.description) {
				return new Validation(this.description["validation"], /*singleValue*/true);
			}
			return null;
		}
	},
	methods: {
		/**
		 * Escape a string to display special characters as non-html interpreted characters.
		 * This is similar to the PHP function htmlentities.
		 */
		toHtmlEntities(str) {
			return String(str).replace(/[\u00A0-\u9999<>&]/gim, (i) => "&#" + i.charCodeAt(0) + ";");
		},
		/**
		 * Extract the type of a content element. Valid types are:
		 * - text
		 * - html
		 */
		getContentType(element) {
			return (typeof element === "object") ? Object.entries(element)[0][0] : "text";
		},
		/**
		 * Extract the data of a display element
		 */
		getContentData(element) {
			return (typeof element === "object") ? Object.entries(element)[0][1] : element;
		},
		/**
		 * Set the different status of the item
		 */
		setError(message) {
			this.$emit("error", message);
		},
		clearError() {
			this.$emit("error", false);
		},
		setActive() {
			if (!this.disable) {

				// Freeze the value
				this.internalIsValueFrozen = true;
				if (this.externalValue instanceof Array) {
					this.internalFrozenValue = this.externalValue.slice(0);
				}
				else if (typeof this.externalValue === "object") {
					this.internalFrozenValue = Object.assign({}, this.externalValue);
				}
				else {
					this.internalFrozenValue = this.externalValue;
				}

				this.isActive = true;
				this.$emit("active", {
					id: this.uid,
					element: this.$el,
					message: this.getOption("description", undefined)
				});
			}
		},
		setInactive() {
			// Unfreeze the value
			this.internalIsValueFrozen = false;
			this.isActive = false;
			this.$emit("active");
		},
		handleActive(context) {
			if (context) {
				this.setActive();
			}
			else {
				this.setInactive();
			}
		},
		/**
		 * Send a submit signal to the form
		 */
		submit() {
			this.$emit("submit");
		},
		/**
		 * Get an attribute from the description
		 */
		getOption(name, defaultValue) {
			if (name in this.description) {
				return this.description[name];
			}
			return defaultValue;
		},
		/**
		 * Return the value
		 */
		get(freezeOnEdit = false) {
			return (freezeOnEdit) ? this.internalValue : this.externalValue;
		},
		async set(value) {
			if (!this.disable) {
				let errorList = [];

				// Validate the value only if it is non-empty
				if (value !== "" && this.validation) {
					const result = this.validation.validate(value, {
						output: "return"
					});
					if ("value" in result) {
						errorList = result.value;
					}
				}

				if (!errorList.length) {
					this.clearError();
					this.$emit("input", value);
					/*
					 * Keep it at the end, it must be called once all the propagation of the value is done.
					 * This to give the opportunity to safely update the global value in this callback.
					 */
					(this.getOption("onchange", () => {}))(value);

					await this.$nextTick();
				}
				else {
					this.setError(errorList);
				}
			}
		},
		// ---- Multi value specific --------------------------------------

		/**
		 * Returns an array of values containing the keys: value, display and search 
		 */
		createMultiValueList(list, isHtml) {
			try {
				// Build the list
				let multiValueList = [];

				if (!list) {
					return [];
				}
				if (typeof list !== "object") {
					list = [list];
				}

				// If it is an array
				if (list instanceof Array) {
					multiValueList = list.map((item, index) => {
						if (typeof item === "object") {
							return { value: item[0], display: item[1], search: ((typeof item[2] === "object") ? item[2] : [String(item[2] || item[1])]).map(item => item.toLowerCase()) };
						}
						return { value: (isHtml) ? index : item, display: item, search: [String(item).toLowerCase()] };
					});
				}

				// If it is an object
				else {
					for (const key in list) {
						multiValueList.push({ value: key, display: list[key], search: [String(list[key]).toLowerCase()] });
					}

					// Sort the list alphabetically
					multiValueList.sort(function(a, b) {
						const valueA = a.value.toLowerCase();
						const valueB = b.value.toLowerCase();
						if (valueA < valueB) {return -1;}
						if (valueA > valueB) {return 1;}
						return 0;
					});
				}

				return multiValueList;
			}
			catch (e) {
				this.setError(e.message);
			}
			return [];
		}
	}
};
</script>
