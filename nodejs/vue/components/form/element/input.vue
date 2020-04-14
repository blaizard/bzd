<template>
	<div class="irform-input" @click="handleClick">
		<span class="irform-input-pre" v-if="pre && getContentType(pre) == 'text'" v-text="getContentData(pre)"></span>
		<span class="irform-input-pre" v-if="pre && getContentType(pre) == 'html'" v-html="getContentData(pre)"></span>

		<span v-if="html" class="irform-input-data">

			<span class="irform-input-multi" v-for="(value, index) in valueList" :key="index">
				<span class="irform-input-multi-item">
					<span v-html="getDisplayValue(value)"></span>
					<span v-if="editable" class="irform-input-multi-delete" @click.stop="valueListRemove(index)">x</span>
				</span>
			</span>
			<span class="irform-input-body"
					ref="input"
					:contenteditable="!disable && editable"
					:tabindex="(disable) ? undefined : 0"
					:data-placeholder="placeholder"
					v-html="valueStr"
					@input="handleInput($event.target.innerText)"
					@focus="setActive()"
					@blur="handleBlur($event, $event.target.innerText)"
					@keydown="handleKeyDown($event, $event.target.innerText)">
			</span>

		</span>
		<span v-else class="irform-input-data">

			<span class="irform-input-multi" v-for="(value, index) in valueList" :key="index">
				<span class="irform-input-multi-item">
					<span v-text="getDisplayValue(value)"></span>
					<span v-if="editable" class="irform-input-multi-delete" @click.stop="valueListRemove(index)">x</span>
				</span>
			</span>
			<span class="irform-input-body"
					ref="input"
					:contenteditable="!disable && editable"
					:tabindex="(disable) ? undefined : 0"
					:data-placeholder="placeholder"
					v-text="valueStr"
					@input="handleInput($event.target.innerText)"
					@focus="setActive()"
					@blur="handleBlur($event, $event.target.innerText)"
					@keydown="handleKeyDown($event, $event.target.innerText)">
			</span>

		</span>

		<span class="irform-input-post" v-if="post && getContentType(post) == 'text'" v-text="getContentData(post)"></span>
		<span class="irform-input-post" v-if="post && getContentType(post) == 'html'" v-html="getContentData(post)"></span>
	</div>
</template>

<script>
	"use strict";

	import Element from "./element.vue"
	export default {
		mixins: [Element],
		props: {
			value: {type: String | Array, required: false, default: ""}
		},
		data: function() {
			return {
				/**
				 * If the displayed values should be interpreted as HTML or not
				 */
				html: this.getOption("html", false),
				/**
				 * Make the content editable
				 */
				editable: this.getOption("editable", true),
				/**
				 * This option will allow any value. If set to false, it will allow
				 * only the preset values.
				 */
				any: this.getOption("any", true),
				/**
				 * Placeholder if no value is input
				 */
				placeholder: this.getOption("placeholder", ""),
				/**
				 * Incorporate a non-editable text preceding the input field
				 */
				pre: this.getOption("pre", false),
				/**
				 * Incorporate a non-editable text after the input field
				 */
				post: this.getOption("post", false),
				/**
				 * Multivalue, this will return an array and not a string
				 */
				multi: this.getOption("multi", false),
				/**
				 * Multivalue separators regular expression
				 */
				multiSeparators: this.getOption("multiSeparators", "[\\s,;]"),
				/**
				 * Values will try to match presets. If any match occurs, It will show
				 * the displayed value.
				 * Presets should follow this format: {value: {display: "Fancy Value"}}.
				 * \note This value is dynamic, hence cannot be from data but from a computed.
				 */
				// presets: {}
				// ---- Internal ----
				hasChanged: false
			}
		},
		computed: {
			valueList() {
				if (this.multi) {
					return (this.get() instanceof Array) ? this.get() : [];
				}
				return [];
			},
			valueStr() {
				if (this.multi) {
					return "";
				}
				return (this.isActive && this.editable) ? this.get() : this.getDisplayValue(this.get());
			},
			presets() {
				return (this.description.hasOwnProperty("presets")) ? this.description["presets"] : {};
			}
		},
		methods: {
			handleClick() {
				this.$refs.input.focus();
			},
			isPreset(value) {
				return this.presets.hasOwnProperty(value);
			},
			isValueAcceptable(value) {
				return this.any || this.isPreset(value);
			},
			getDisplayValue(value) {
				return (this.isPreset(value)) ? this.presets[value] : value;
			},
			valueListAdd(text) {
				const regexpr =  "^" + this.multiSeparators + "+|" + this.multiSeparators + "+$";
				const cleanedValue = text.replace(new RegExp(regexpr, "g"), "");
				if (cleanedValue && this.isValueAcceptable(cleanedValue)) {
					this.set(this.valueList.concat(cleanedValue));
				}
				this.$refs.input.innerHTML = "";
				this.$emit("directInput", "");
			},
			valueSet(text) {
				if (this.isValueAcceptable(text)) {
					this.set(text);
				}
				else {
					this.$refs.input.innerHTML = "";
					this.$emit("directInput", "");
					this.set("");
				}
			},
			valueListRemoveLast() {
				let valueList = this.valueList.slice(0);
				valueList.pop();
				this.set(valueList);
			},
			valueListRemove(index) {
				let valueList = this.valueList.slice(0);
				valueList.splice(index, 1);
				this.set(valueList);
			},
			handleKeyDown(e, text) {
				// If press ENTER
				if (e.keyCode == 13) {
					e.preventDefault();
					if (this.multi) {
						this.valueListAdd(text);
					}
					else {
						this.submit();
						this.valueSet(text);
						this.$refs.input.blur();
					}
				}
				// If press Backspace
				else if (e.keyCode == 8) {
					if (this.multi && text === "") {
						this.valueListRemoveLast();
					}
				}
				// Emit special keys
				else if (e.keyCode < 46 && [/*tab*/9, 32, /*arrow left*/37, /*arrow right*/39].indexOf(e.keyCode) === -1) {
					e.preventDefault();
					this.$emit("key", e.keyCode);
				}
			},
			handleInput(text) {
				this.hasChanged = true;
				this.$emit("directInput", text);

				if (this.multi) {
					const lastChar = text[text.length - 1] || "";
					if (lastChar.match(new RegExp(this.multiSeparators, "g"))) {
						this.valueListAdd(text);
					}
				}
			},
			handleBlur(e, text) {
				if (this.hasChanged) {
					if (this.multi) {
						this.valueListAdd(text);
					}
					else {
						this.valueSet(text);
					}
					this.hasChanged = false;
				}
				this.setInactive();
			}
		}
	}
</script>
