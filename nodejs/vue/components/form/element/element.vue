<script>
	import Validation from "../../../../core/validation.mjs";
	import ExceptionFactory from "../../../../core/exception.mjs";

	const Exception = ExceptionFactory("form");
	let uidCounter = 0;

	/// The following must be implement for all elements:
	/// Events:
	/// - @active: when the element has the focus
	/// - @update:model-value: when the element has a new value
	/// - @error: when the element failed
	/// Props:
	/// - disable: set the element as disabled
	///
	/// It handles the following keys from the "description" property:
	/// - validation: syntax compatible with validation module.
	/// - valueType: a string defining the type of data present. It can be "number", "list" or "any".
	export default {
		props: {
			modelValue: { required: false, default: null },
			description: { type: Object, required: false, default: () => ({}) },
			disable: { type: Boolean, default: false, required: false },
			/// The context of the element, used to get extra information about the
			/// position and surounding of this element.
			context: { type: Object, default: {}, required: false },
			onUpdateWithContext: { default: false, required: false },
		},
		emits: ["error", "active", "submit", "update-with-context", "update:model-value"],
		data: function () {
			return {
				uid: "irform-element-" + ++uidCounter,
				isInputFrozenValue: false,
				inputFrozenValue: "",
				isActive: false,
			};
		},
		computed: {
			hasListenerInputWithContext() {
				return Boolean(this.onUpdateWithContext);
			},
			valueType() {
				return "any";
			},
			/// Return a functor to convert the internal value into external.
			toOutputValue() {
				const toOutputValue = this.getOption("toOutputValue", this.valueType);
				if (typeof toOutputValue == "string") {
					switch (toOutputValue) {
						case "list_to_map":
							return (v) => {
								return v.reduce((object, value) => {
									let copyValue = Object.assign({}, value);
									delete copyValue.key;
									object[value.key] = copyValue;
									return object;
								}, {});
							};
						case "any":
						case "list":
						case "string":
							return (v) => v;
						case "boolean":
							return (v) => Boolean(v);
						case "number":
							return (v) => Number(v) || 0;
						default:
							this.setError('Unknown value preset "' + toOutputValue + '"');
					}
				} else if (typeof toOutputValue == "function") {
					return toOutputValue;
				}
				this.setError('Unknown output type "' + toOutputValue + '", must be a string or function');
			},
			/// Return a functor to convert the external value into internal.
			toInputValue() {
				const toInputValue = this.getOption("toInputValue", this.valueType);
				if (typeof toInputValue == "string") {
					switch (toInputValue) {
						case "map_to_list":
							return (v) => {
								return Object.entries(v || {}).map(([key, value]) => {
									return Object.assign({}, value, { key: key });
								});
							};
						case "any":
							return (v) => v;
						case "number":
							return (v) => v || 0;
						case "string":
							return (v) => v || "";
						case "boolean":
							return (v) => v || false;
						case "list":
							return (v) => v || [];
						default:
							this.setError('Unknown value preset "' + toInputValue + '"');
					}
				} else if (typeof toInputValue == "function") {
					return toInputValue;
				}
				this.setError('Unknown input type "' + toInputValue + '", must be a string or function');
			},
			inputValue() {
				const value = this.toInputValue(this.modelValue);
				const errorMessage = [
					"The value must be of type '{}', instead value is '{}' of type '{}'.",
					this.valueType,
					value,
					typeof value,
				];
				switch (this.valueType) {
					case "list":
						Exception.assert(value instanceof Array, ...errorMessage);
						break;
					case "string":
						Exception.assert(typeof value == "string", ...errorMessage);
						break;
					case "number":
						Exception.assert(typeof value == "number", ...errorMessage);
						break;
					case "boolean":
						Exception.assert(typeof value == "boolean", ...errorMessage);
						break;
					default:
				}
				return value;
			},
			validation() {
				if ("validation" in this.description) {
					return new Validation(this.description["validation"], /*singleValue*/ true);
				}
				return null;
			},
		},
		methods: {
			/// Escape a string to display special characters as non-html interpreted characters.
			/// This is similar to the PHP function htmlentities.
			toHtmlEntities(str) {
				return String(str).replace(/[\u00A0-\u9999<>&]/gim, (i) => "&#" + i.charCodeAt(0) + ";");
			},
			/// Extract the type of a content element. Valid types are:
			/// - text
			/// - html
			getContentType(element) {
				return typeof element === "object" ? Object.entries(element)[0][0] : "text";
			},
			/// Extract the data of a display element
			getContentData(element) {
				return typeof element === "object" ? Object.entries(element)[0][1] : element;
			},
			/// Set the different status of the item
			setError(message) {
				this.$emit("error", message);
			},
			clearError() {
				this.$emit("error", false);
			},
			setActive() {
				if (!this.disable) {
					// Freeze the value
					this.isInputFrozenValue = true;
					if (this.inputValue instanceof Array) {
						this.inputFrozenValue = this.inputValue.slice(0);
					} else if (typeof this.inputValue === "object") {
						this.inputFrozenValue = Object.assign({}, this.inputValue);
					} else {
						this.inputFrozenValue = this.inputValue;
					}

					this.isActive = true;
					this.$emit("active", {
						id: this.uid,
						element: this.$el,
						message: this.getOption("description", undefined),
					});
				}
			},
			setInactive() {
				// Unfreeze the value
				this.isInputFrozenValue = false;
				this.isActive = false;
				this.$emit("active");
			},
			handleActive(context) {
				if (context) {
					this.setActive();
				} else {
					this.setInactive();
				}
			},
			/// Send a submit signal to the form
			submit() {
				this.$emit("submit", this.context);
			},
			/// Get an attribute from the description
			getOption(name, defaultValue) {
				if (name in this.description) {
					return this.description[name];
				}
				return defaultValue;
			},
			/// Return the value
			get(freezeOnEdit = false) {
				if (freezeOnEdit && this.isInputFrozenValue) {
					return this.inputFrozenValue;
				}
				return this.inputValue;
			},
			/// Assign a new value for this element.
			///
			/// \param value The new value to be set.
			/// \param context Extra context to be returned with this.context.
			async set(value, context = {}) {
				if (!this.disable) {
					let errorList = [];

					// Update the value
					const outputValue = this.toOutputValue(value);

					// Validate the value only if it is non-empty
					if (outputValue !== "" && this.validation) {
						const result = this.validation.validate(outputValue, {
							output: "return",
						});
						if ("value" in result) {
							errorList = result.value;
						}
					}

					if (!errorList.length) {
						this.clearError();

						const updateContext = Object.assign({}, this.context, context);
						if (this.hasListenerInputWithContext) {
							this.$emit("update-with-context", { value: outputValue, context: updateContext });
						} else {
							this.$emit("update:model-value", outputValue);
						}
						// Keep it at the end, it must be called once all the propagation of the value is done.
						// This to give the opportunity to safely update the global value in this callback.
						this.getOption("onchange", () => {})(outputValue, updateContext);

						await this.$nextTick();
					} else {
						this.setError(errorList);
					}
				}
			},
			// ---- Multi value specific --------------------------------------

			/// Returns an array of values containing the keys: value, display and search
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
								return {
									value: item[0],
									display: item[1],
									search: (typeof item[2] === "object" ? item[2] : [String(item[2] || item[1])]).map((item) =>
										item.toLowerCase(),
									),
								};
							}
							return { value: isHtml ? index : item, display: item, search: [String(item).toLowerCase()] };
						});
					}

					// If it is an object
					else {
						for (const key in list) {
							multiValueList.push({ value: key, display: list[key], search: [String(list[key]).toLowerCase()] });
						}

						// Sort the list alphabetically
						multiValueList.sort(function (a, b) {
							const valueA = a.value.toLowerCase();
							const valueB = b.value.toLowerCase();
							if (valueA < valueB) {
								return -1;
							}
							if (valueA > valueB) {
								return 1;
							}
							return 0;
						});
					}

					return multiValueList;
				} catch (e) {
					this.setError(e.message);
				}
				return [];
			},
		},
	};
</script>
