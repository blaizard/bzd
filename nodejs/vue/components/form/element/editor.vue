<template>
	<div :class="containerClass">
		<ElementCarousel class="irform-editor-toolbar" :disable="disable" :description="toolbarDescription">
		</ElementCarousel>
		<ElementTextarea
			class="irform-editor-content"
			tag="div"
			ref="content"
			@active="handleActive"
			@update:model-value="handleUpdate"
			:disable="disable"
			:contenteditable="!disable"
			:description="contentDescription"
			:data-placeholder="placeholder"
			v-html="get(/*freezeOnEdit*/ true)"
		>
		</ElementTextarea>
	</div>
</template>

<script>
	import Element from "./element.vue";
	import ElementCarousel from "./carousel.vue";
	import ElementTextarea from "./textarea.vue";

	export default {
		mixins: [Element],
		components: {
			ElementCarousel,
			ElementTextarea,
		},
		data: function () {
			return {
				toolbar: [
					"undo",
					"separator",
					"bold",
					"italic",
					"underline",
					"strike",
					"separator",
					"justifyLeft",
					"justifyCenter",
					"justifyRight",
					"justify",
					"separator",
					"indent",
					"unindent",
					"orderedList",
					"unorderedList",
					"quote",
					"separator",
					"clearFormat",
				],
				history: [],
				historyMax: 20,
			};
		},
		computed: {
			/// ---- CONFIG ----------------------------------------
			placeholder() {
				return this.getOption("placeholder", "");
			},
			/// ---- IMPLEMENTATION ----------------------------------
			valueType() {
				return "string";
			},
			containerClass() {
				return {
					"irform-editor": true,
					[this.getOption("class")]: true,
				};
			},
			toolbarElements() {
				const supportedElements = {
					separator: {
						html: '<div class="irform-editor-toolbar-separator"></div>',
					},
					bold: {
						html: '<i class="bzd-icon-bold"></i>',
						type: "button",
						click: () => this.actionOnSelection("bold"),
					},
					italic: {
						html: '<i class="bzd-icon-italic"></i>',
						type: "button",
						click: () => this.actionOnSelection("italic"),
					},
					underline: {
						html: '<i class="bzd-icon-underline"></i>',
						type: "button",
						click: () => this.actionOnSelection("underline"),
					},
					strike: {
						html: '<i class="bzd-icon-strike_through"></i>',
						type: "button",
						click: () => this.actionOnSelection("strikeThrough"),
					},
					undo: {
						html: '<i class="bzd-icon-undo"></i>',
						type: "button",
						click: () => {
							if (this.history.length) {
								this.set(this.history.shift(), { action: "update" });
							}
						},
					},
					justifyLeft: {
						html: '<i class="bzd-icon-justify_left"></i>',
						type: "button",
						click: () => this.action("justifyLeft"),
					},
					justifyCenter: {
						html: '<i class="bzd-icon-justify_center"></i>',
						type: "button",
						click: () => this.action("justifyCenter"),
					},
					justifyRight: {
						html: '<i class="bzd-icon-justify_right"></i>',
						type: "button",
						click: () => this.action("justifyRight"),
					},
					justify: {
						html: '<i class="bzd-icon-justify"></i>',
						type: "button",
						click: () => this.action("justifyFull"),
					},
					indent: {
						html: '<i class="bzd-icon-indent"></i>',
						type: "button",
						click: () => this.action("indent"),
					},
					unindent: {
						html: '<i class="bzd-icon-unindent"></i>',
						type: "button",
						click: () => this.action("outdent"),
					},
					orderedList: {
						html: '<i class="bzd-icon-ordered_list"></i>',
						type: "button",
						click: () => this.action("insertOrderedList"),
					},
					unorderedList: {
						html: '<i class="bzd-icon-unordered_list"></i>',
						type: "button",
						click: () => this.action("insertUnorderedList"),
					},
					quote: {
						html: '<i class="bzd-icon-quote"></i>',
						type: "button",
						click: () => this.action("formatBlock", "blockquote"),
					},
					clearFormat: {
						html: '<i class="bzd-icon-clear_format"></i>',
						type: "button",
						click: () => this.actionRemoveFormat(),
					},
				};

				return this.toolbar.map((element) => supportedElements[element]);
			},
			toolbarDescription() {
				return {
					click: (index) => {
						if (typeof this.toolbarElements[index].click === "function") {
							this.toolbarElements[index].click();
						}
					},
					controls: "none",
					alignSelection: false,
					list: this.toolbarElements.map((element) => this.generateToolbarElement(element)),
				};
			},
			contentDescription() {
				return Object.assign({}, this.description);
			},
			contentElement() {
				return this.$refs.content.$el;
			},
		},
		methods: {
			generateToolbarElement(element) {
				switch (element.type) {
					case "button":
						return '<div class="irform-editor-toolbar-element">' + element.html + "</div>";
					default:
						return element.html;
				}
			},
			isTextSelected() {
				return window.getSelection().toString() != "";
			},
			selectAll() {
				let range = document.createRange();
				let sel = window.getSelection();
				range.selectNodeContents(this.$refs.content.$el);
				sel.removeAllRanges();
				sel.addRange(range);
			},
			unselect() {
				let sel = window.getSelection();
				sel.removeAllRanges();
			},
			action(actionStr, value) {
				document.execCommand(actionStr, false, value || null);
				this.contentElement.dispatchEvent(new Event("update:model-value"));
			},
			actionOnSelection(actionStr, value) {
				const isSelected = this.isTextSelected();
				isSelected || this.selectAll();
				this.action(actionStr, value);
				isSelected || this.unselect();
			},
			actionRemoveFormat() {
				if (this.isTextSelected()) {
					document.execCommand("removeFormat", false, null);
				} else {
					this.contentElement.innerHTML = this.contentElement.innerText;
					this.contentElement.dispatchEvent(new Event("update:model-value"));
				}
			},
			pushToHistory(value) {
				if (!this.history.length || this.history[0] != value) {
					this.history.unshift(value);
					this.history.length = Math.min(this.history.length, this.historyMax);
				}
			},
			handleUpdate() {
				const value = this.contentElement.innerHTML;
				// Save the previous value into the history
				this.pushToHistory(this.get());
				// Save the value
				this.set(value, { action: "update" });
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/styles/default/css/form/editor.scss" as *;
</style>
