<script>
	"use strict";

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
	 * - validate: a string, a list of string or functions to validate the data.
	 * - valueType: a string defining the type of data present. It can be "number", "list" or "any".
	 */
	export default {
		props: {
			description: {type: Object, required: true, default: {}},
			disable: {type: Boolean, default: false, required: false}
		},
		data: function() {
			return {
				uid: this._uid,
				internalIsValueFrozen: false,
				internalFrozenValue: "",
				isActive: false
			}
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
					break;
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
			validateFctList() {
				let validate = this.getOption("validate", null);

				// If there is no validate set, set the default one if any
				if (validate === null) {
					switch (this.valueType) {
					case "number":
						validate = "number";
						break;
					}
				}

				if (typeof validate === "function") {
					return [validate];
				}

				if (typeof validate === "string") {
					validate = validate.split(",");
				}
				if (validate instanceof Array) {
					return validate.map((value) => {
						return this.getValidateFct(value);
					});
				}
				return [];
			}
		},
		methods: {
			getValidateFct(validate) {
				if (typeof validate === "function") {
					return validate;
				}
				else if (typeof validate === "string") {
					switch (validate.trim()) {
					case "number":
						return (value) => (value.match(/^-?[0-9]+(?:\.[0-9]*){0,1}$/)) ? true : "Must be a number";
					case "integer":
						return (value) => (value.match(/^-?[0-9]+$/)) ? true : "Must be an integer";
					case "positive":
						return (value) => ((parseFloat(value) || 0) >= 0) ? true : "Must be positive";
					case "email":
						return (value) => (value.match(/^(([^<>()\[\]\\.,;:\s@"]+(\.[^<>()\[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/i)) ? true : "Wrong email format";
					default:
						console.error("Unknown validate value \"" + validate + "\"");
					}
				}
				return () => true;
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
				if (this.description.hasOwnProperty(name)) {
					return this.description[name];
				}
				return defaultValue;
			},
			/**
			 * Return the value
			 */
			get(freezeOnEdit) {
				return (freezeOnEdit) ? this.internalValue : this.externalValue;
			},
			set(value) {
				if (!this.disable) {
					let errorList = [];

					// Validate the value only if it is non-empty
					if (value !== "") {
						this.validateFctList.forEach((fct) => {
							const result = fct(value);
							if (result !== true) {
								errorList.push(result);
							}
						});
					}

					if (!errorList.length) {
						this.clearError();
						(this.getOption("onchange", () => {}))(value);
						this.$emit("input", value);
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
								return { value: item[0], display: item[1], search: ((typeof item[2] === "object") ? item[2] : [String(item[2] || item[1])]).map(item => item.toLowerCase()) }
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
							if (valueA < valueB) return -1;
							if (valueA > valueB) return 1;
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
	}
</script>
