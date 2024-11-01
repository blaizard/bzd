<template>
	<div :class="containerClass" @click="handleClick">
		<span class="irform-input-pre" v-if="pre && getContentType(pre) == 'text'" v-text="getContentData(pre)"></span>
		<span class="irform-input-pre" v-if="pre && getContentType(pre) == 'html'" v-html="getContentData(pre)"></span>

		<span class="irform-input-data">
			<span class="irform-input-multi" v-for="(value, index) in valueList" :key="index">
				<span class="irform-input-multi-item">
					<span v-html="getDisplayValue(value, /*editable*/ false)"></span>
					<span class="irform-input-multi-delete" @click.stop="valueListRemove(index)">x</span>
				</span>
			</span>

			<span
				class="irform-input-body"
				ref="input"
				:contenteditable="!disable && editable"
				:tabindex="disable ? undefined : 0"
				:data-placeholder="isActive ? '' : placeholder"
				v-html="valueStr"
				@input="handleInput($event.target.innerText)"
				@focus="handleFocus($event)"
				@blur="handleBlur($event, $event.target.innerText)"
				@keydown="handleKeyDown($event, $event.target.innerText)"
			>
			</span>

			<span
				v-if="mask"
				ref="maskInput"
				type="text"
				v-text="get()"
				style="width: 1px; height: 1px; overflow: hidden; opacity: 0"
				:contenteditable="editable"
				@focus="handleFocus($event)"
				@input="handleInputMask()"
				@blur="handleBlur($event, $event.target.innerText)"
				@keydown="handleKeyDown($event, $event.target.innerText)"
			>
			</span>
		</span>

		<span class="irform-input-post" v-if="post && getContentType(post) == 'text'" v-text="getContentData(post)"></span>
		<span class="irform-input-post" v-if="post && getContentType(post) == 'html'" v-html="getContentData(post)"></span>
	</div>
</template>

<script>
	import Element from "./element.vue";
	export default {
		mixins: [Element],
		emits: ["directInput", "key"],
		data: function () {
			return {
				hasChanged: false,
			};
		},
		computed: {
			/// ---- CONFIG ----------------------------------------
			/// If the displayed values should be interpreted as HTML or not
			html() {
				return this.getOption("html", false);
			},
			/// Make the content editable
			editable() {
				return this.getOption("editable", true);
			},
			/// This option will allow any value. If set to false, it will allow
			/// only the preset values.
			any() {
				return this.getOption("any", true);
			},
			/// Format the displayed value.
			/// The formatting can be:
			/// - A string -> mask: ***-***
			/// - A Function -> format (value) => (...)
			/// - A Dictionaty -> presets: { "hello" => "<i>hello</i>" }
			format() {
				return this.getOption("format", false);
			},
			/// Set if the real value should be masked
			mask() {
				return this.getOption("mask", false);
			},
			/// Placeholder if no value is input
			placeholder() {
				return this.getOption("placeholder", "");
			},
			/// Incorporate a non-editable text preceding the input field
			pre() {
				return this.getOption("pre", false);
			},
			/// Incorporate a non-editable text after the input field
			post() {
				return this.getOption("post", false);
			},
			/// Multivalue, this will return an array and not a string
			multi() {
				return this.getOption("multi", false);
			},
			/// Multivalue separators regular expression
			multiSeparators() {
				return this.getOption("multiSeparators", "[\\s,;]");
			},
			/// ---- IMPLEMENTATION ----------------------------------
			valueType() {
				return this.multi ? "list" : this.getOption("valueType", "string");
			},
			containerClass() {
				return {
					"irform-input": true,
					[this.getOption("class")]: true,
				};
			},
			valueList() {
				if (this.multi) {
					return this.get() instanceof Array ? this.get() : [];
				}
				return [];
			},
			valueStr() {
				if (this.multi) {
					return "";
				}
				return this.getDisplayValue(this.get(), /*editable*/ true);
			},
		},
		methods: {
			handleClick() {
				this.$refs.input.focus();
			},
			isValueAcceptable(/*value*/) {
				return this.any;
			},
			formatValue(value) {
				if (this.format !== false) {
					if (typeof this.format == "function") {
						return this.format(value);
					} else if (typeof this.format == "string") {
						let index = 0;
						return [...this.format].reduce((formatted, c) => {
							switch (c) {
								case "*":
									formatted += value[index++] || "_";
									break;
								default:
									formatted += c;
							}
							return formatted;
						}, "");
					} else if (typeof this.format == "object") {
						if (value in this.format) {
							return this.format[value];
						}
					}
				}
				return value;
			},
			getDisplayValue(value, editable) {
				const formattedValue =
					!this.mask && this.isActive && this.editable && editable ? value : this.formatValue(value);
				return this.html ? formattedValue : this.toHtmlEntities(formattedValue);
			},
			valueListAdd(text) {
				const regexpr = "^" + this.multiSeparators + "+|" + this.multiSeparators + "+$";
				const cleanedValue = text.replace(new RegExp(regexpr, "g"), "");
				if (cleanedValue && this.isValueAcceptable(cleanedValue)) {
					this.set(this.valueList.concat(cleanedValue), { action: "add" });
				}
				this.$refs.input.innerHTML = "";
				this.$emit("directInput", "");
			},
			valueSet(text) {
				if (this.isValueAcceptable(text)) {
					this.set(text, { action: "update" });
				} else {
					this.$refs.input.innerHTML = "";
					this.$emit("directInput", "");
					this.set("", { action: "update" });
				}
			},
			valueListRemoveLast() {
				let valueList = this.valueList.slice(0);
				const value = valueList.pop();
				this.set(valueList, { action: "remove", value: value });
			},
			valueListRemove(index) {
				let valueList = this.valueList.slice(0);
				const value = valueList.pop(index);
				this.set(valueList, { action: "remove", value: value });
			},
			handleKeyDown(e, text) {
				// If press ENTER
				if (e.keyCode == 13) {
					e.preventDefault();
					if (this.multi) {
						this.valueListAdd(text);
					} else {
						this.valueSet(text);
						this.$refs.input.blur();
						this.submit(this.context);
					}
				}
				// If press Backspace
				else if (e.keyCode == 8) {
					if (this.multi && text === "") {
						this.valueListRemoveLast();
					}
				}
				// Emit special keys
				else if (e.keyCode < 46 && [/*tab*/ 9, 32].indexOf(e.keyCode) == -1) {
					if ([/*arrow left*/ 37, /*arrow right*/ 39].indexOf(e.keyCode) == -1) {
						e.preventDefault();
					}
					this.$emit("key", e.keyCode);
				}
			},
			// getCaretPosition(element) {
			// const sel = window.getSelection();
			// let offset = 0;
			// if (sel.rangeCount) {
			// const range = sel.getRangeAt(0);
			// const origOffset = offset = range.startOffset;
			// let parent = range.commonAncestorContainer.parentNode;
			// while (parent && parent.parentNode && parent != element) {
			// 	offset = Array.prototype.indexOf.call(parent.parentNode.children, parent) + origOffset;
			// 	parent = parent.parentNode;
			// }
			// }
			// return offset;
			// },
			setCaretPosition(element, position) {
				if (!element.childNodes[0]) {
					return;
				}
				let range = document.createRange();
				let sel = window.getSelection();
				const length = element.innerText.length;
				const start = Math.min(position, length);
				range.setStart(element.childNodes[0], start);
				range.collapse(true);
				sel.removeAllRanges();
				sel.addRange(range);
			},
			handleInput(text) {
				this.hasChanged = true;
				this.$emit("directInput", text);

				if (this.mask) {
					const maskedText = this.getDisplayValue(text, /*editable*/ true);
					this.$refs.input.innerHTML = maskedText;
				}

				if (this.multi) {
					const lastChar = text[text.length - 1] || "";
					if (lastChar.match(new RegExp(this.multiSeparators, "g"))) {
						this.valueListAdd(text);
					}
				}
			},
			handleInputMask() {
				const text = this.$refs.maskInput.innerText;
				this.handleInput(text);
			},
			handleFocus() {
				this.setActive();
				if (this.mask) {
					this.$refs.maskInput.focus();
					this.setCaretPosition(this.$refs.maskInput, 10000);
				}
			},
			handleBlur(e, text) {
				if (this.hasChanged) {
					// Safari uses '\n' when the user removed everything with backspace.
					// This keep a consistent behavior with other browsers.
					text = text.trim();
					if (this.multi) {
						this.valueListAdd(text);
					} else {
						this.valueSet(text);
					}
					this.hasChanged = false;
				}
				this.setInactive();
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/styles/default/css/form/input.scss" as *;
</style>
