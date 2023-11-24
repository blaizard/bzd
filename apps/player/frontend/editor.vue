<template>
	<div class="editor">
		<code
			ref="editor"
			contenteditable="plaintext-only"
			spellcheck="false"
			@keydown="handleKeyDown"
			@input="handleInput"
		></code>
	</div>
</template>

<script>
	import "highlight.js/styles/github.css";
	import HighlightJs from "highlight.js/lib/common";

	function getTextNodeAtPosition(root, index) {
		const NODE_TYPE = NodeFilter.SHOW_TEXT;
		let treeWalker = document.createTreeWalker(root, NODE_TYPE, (elem) => {
			if (index > elem.textContent.length) {
				index -= elem.textContent.length;
				return NodeFilter.FILTER_REJECT;
			}
			return NodeFilter.FILTER_ACCEPT;
		});
		const c = treeWalker.nextNode();
		return {
			node: c ? c : root,
			position: index,
		};
	}

	export default {
		props: {
			value: { type: String, mandatory: true },
			path: { type: String, mandatory: false, default: "" },
			focus: { type: Boolean, mandatory: false, default: false },
			caret: { type: Number, mandatory: false, default: 0 },
		},
		data: function () {
			return {
				currentPath: this.path,
				updateRequested: null,
				/// If null, it will use the existing position, otherwise, if it is a number, it will use it.
				caretPosition: null,
			};
		},
		watch: {
			value: {
				async handler() {
					// Wait until the editor is up.
					await this.$nextTick();
					// If the path are different and an update is requested.
					if (this.currentPath != this.path) {
						this.caretPosition = null;
						if (this.updateRequested) {
							this.update();
						}
					} else if (this.updateRequested) {
						this.cancelUpdate();
					}

					this.currentPath = this.path;
					const html = this.language
						? HighlightJs.highlight(this.value, { language: this.language }).value
						: this.textToHTML(this.value);
					const position = this.caretPosition === null ? this.getCaretPosition() : this.caretPosition;
					this.$refs.editor.innerHTML = html;
					this.setCaretPosition(position);
				},
				immediate: true,
			},
			focus: {
				handler() {
					if (this.focus) {
						this.$refs.editor.focus();
					}
				},
				immediate: true,
			},
			caret: {
				handler() {
					this.caretPosition = this.caret;
				},
				immediate: true,
			},
		},
		mounted() {
			document.addEventListener("selectionchange", this.handleSelectionChange);
		},
		beforeUnmount() {
			document.removeEventListener("selectionchange", this.handleSelectionChange);
		},
		computed: {
			language() {
				const ext = this.path.split("/").pop().split(".").pop().toLowerCase();
				switch (ext) {
					case "build":
					case "workspace":
					case "bzl":
						return "py";
					default:
						if (HighlightJs.getLanguage(ext)) {
							return ext;
						}
				}
				return "";
			},
		},
		emits: ["input"],
		methods: {
			getCaretPosition() {
				const selection = window.getSelection();
				if (selection.rangeCount > 0) {
					let range = selection.getRangeAt(0);
					const clone = range.cloneRange();
					range.setStart(this.$refs.editor, 0);
					const position = range.toString().length;
					selection.removeAllRanges();
					selection.addRange(clone);
					return position;
				}
				return 0;
			},
			setCaretPosition(position) {
				const data = getTextNodeAtPosition(this.$refs.editor, position);
				const selection = window.getSelection();
				selection.removeAllRanges();
				let range = new Range();
				try {
					range.setStart(data.node, data.position);
					selection.addRange(range);
				} catch (e) {
					if (e.name == "IndexSizeError") {
						// Ignore.
					} else {
						throw e;
					}
				}
			},
			insertText(text) {
				const sel = document.getSelection();
				let range = sel.getRangeAt(0);
				const node = document.createTextNode(text);
				range.insertNode(node);
				range.setStartAfter(node);
				range.setEndAfter(node);
				// Trigger the input event.
				this.handleInput();
			},
			textToHTML(text) {
				return text
					.replace(/&/g, "&amp;")
					.replace(/</g, "&lt;")
					.replace(/>/g, "&gt;")
					.replace(/"/g, "&quot;")
					.replace(/'/g, "&#039;");
			},
			cancelUpdate() {
				clearTimeout(this.updateRequested);
				this.updateRequested = null;
			},
			update() {
				const value = this.$refs.editor.textContent;
				this.$emit("input", { path: this.currentPath, content: value });
				this.cancelUpdate();
			},
			handleKeyDown(e) {
				// tab.
				if (e.keyCode == 9) {
					e.preventDefault();
					this.insertText("\u0009");
				}
			},
			handleSelectionChange() {
				this.caretPosition = null;
			},
			handleInput() {
				this.cancelUpdate();
				this.updateRequested = setTimeout(this.update, 1000);
			},
		},
	};
</script>

<style lang="scss" scoped>
	.editor {
		code {
			padding-left: 3ch;
			display: inline-block;
			border: none;
			outline: 0px solid transparent;
		}

		.line-number {
			width: 3ch;
			display: inline-block;
			text-align: right;
			padding: 0;
			margin: 0;
			margin-left: -3ch;
			margin-right: 3ch;
			color: #999;
			user-select: none;
		}
	}
</style>
