<script>
	import Validation from "../../../../core/validation.mjs";

	/// The following must be implement for all elements:
	/// Events:
	/// - @active: when the element has the focus
	/// - @input: when the element has a new value
	/// - @error: when the element failed
	/// Props:
	/// - disable: set the element as disabled
	///
	/// It handles the following keys from the "description" property:
	/// - validation: syntax compatible with validation module.
	/// - valueType: a string defining the type of data present. It can be "number", "list" or "any".
	export default {
		props: {
			description: { type: Object, required: false, default: () => ({}) },
			disable: { type: Boolean, default: false, required: false },
			/// The context of the element, used to get extra information about the
			/// position and surounding of this element.
			context: { type: Object, default: {}, required: false },
		},
		data: function () {
			return {
				uid: this._uid,
				isInputFrozenValue: false,
				inputFrozenValue: "",
				isActive: false,
			};
		},
		computed: {
			hasListenerInputWithContext() {
				return Boolean(this.$listeners && "input-with-context" in this.$listeners);
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
						case "number":
							return (v) => v;
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
						case "list":
							return (v) => {
								let list = v || [];
								if (!(list instanceof Array)) {
									this.setError("The value must be an Array");
									list = [];
								}
								return list;
							};
						case "map_to_list":
							return (v) => {
								return Object.entries(v).map(([key, value]) => {
									return Object.assign({}, value, { key: key });
								});
							};
						case "any":
						case "number":
							return (v) => v;
						default:
							this.setError('Unknown value preset "' + toInputValue + '"');
					}
				} else if (typeof toInputValue == "function") {
					return toInputValue;
				}
				this.setError('Unknown input type "' + toInputValue + '", must be a string or function');
			},
			inputValue() {
				return this.toInputValue(this.value);
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

						const updateContext = Object.assign(
							{
								previous: this.value,
							},
							this.context,
							context,
						);
						if (this.hasListenerInputWithContext) {
							this.$emit("input-with-context", { value: outputValue, context: updateContext });
						} else {
							this.$emit("input", outputValue);
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
