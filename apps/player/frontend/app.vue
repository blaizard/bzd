<template>
	<div class="layout">
		<div class="tree">
			<Tree :list="tree" @selected="handleTreeSelected"></Tree>
		</div>
		<div class="content">
			<code v-if="files.selected" v-html="contentHTML"></code>
		</div>
		<Terminal class="terminal" :stream="terminal" @processed="handleTerminalProcessed"></Terminal>
	</div>
</template>

<script>
	import Layout from "bzd/vue/components/layout/layout.vue";
	import Tree from "./tree.vue";
	import Terminal from "./terminal.vue";
	import Scenario from "../lib/scenario.mjs";
	import FileSystem from "../lib/filesystem.mjs";
	import "highlight.js/styles/github.css";
	import HighlightJs from "highlight.js/lib/common";
	import audioSrc from "./assets/typing/click.mp3";

	export default {
		components: {
			Layout,
			Tree,
			Terminal,
		},
		data: function () {
			return {
				start: 0,
				index: 0,
				scenario: new Scenario(),
				files: new FileSystem(this.$api),
				selection: [0, 0],
				terminal: [],
			};
		},
		computed: {
			completed() {
				return this.index >= this.scenario.size;
			},
			action() {
				return this.scenario.at(this.index);
			},
			fastforward() {
				return this.index < this.start;
			},
			tree() {
				return [{ name: this.scenario.name, expanded: true, children: this.files.makeTree() }];
			},
			content() {
				if (this.files.selected) {
					return this.files.selected.content;
				}
				return "";
			},
			contentHTML() {
				// Replace the selection tags.
				const content = this.contentHightlighted.replace().replace();
				return content
					.split("\n")
					.map((line, index) => "<span class=\"line-number\">" + index + "</span>" + line)
					.join("\n");
			},
			contentHightlighted() {
				const first = this.content.slice(0, this.selection[0]);
				const selected = this.content.slice(this.selection[0], this.selection[1]);
				const last = this.content.slice(this.selection[1]);
				const hightlightedFirst = HighlightJs.highlight(first, { language: "cc" }).value;
				const hightlightedLast = HighlightJs.highlight(last, { language: "cc" }).value;
				return hightlightedFirst + "<span class=\"selection\">" + selected + "</span>" + hightlightedLast;
			},
		},
		async mounted() {
			this.scenario = new Scenario(await this.$api.request("get", "/scenario"));
			this.$routerSet({
				ref: "view",
				routes: [
					{ path: "/", handler: this.execute },
					{
						path: "/{index}",
						handler: (path, options, args) => {
							this.start = Math.max(this.start, args.index);
							this.execute();
						},
					},
				],
			});
		},
		methods: {
			sleep(ms) {
				if (!this.fastforward) {
					return new Promise((resolve) => setTimeout(resolve, ms));
				}
			},
			setSelectionEnd() {
				this.selection = [this.content.length, this.content.length];
			},
			async emulateTyping(object, keyName, value) {
				for (const c of value) {
					await this.emulateTypingKey(c);
					object[keyName] += c;
					this.setSelectionEnd();
				}
			},
			async emulateTypingTerminal(value) {
				for (const c of value) {
					await this.emulateTypingKey(c);
					this.terminal.push(c);
				}
			},
			async emulateTypingKey(key) {
				let audio = new Audio(audioSrc);
				await this.sleep(10 + Math.random() * 200);
				audio.volume = (key.charCodeAt(0) % 10) / 10 + 0.1;
				audio.play();
			},
			async executeFileCreate(path) {
				await this.files.createFile(path, "");
				await this.emulateTyping(this.files.selected.node, "name", FileSystem.basename(path));
			},
			async executeFileWrite(path, content) {
				if (!this.files.select(path)) {
					await this.executeFileCreate(path);
				}
				await this.files.selected.updateContent(async () => {
					await this.emulateTyping(this.files.selected, "content", content);
				});
			},
			async executeFileSelect(path) {
				this.files.select(path);
				await this.files.selected.fetchContent();
				this.setSelectionEnd();
			},
			async *streamAsyncIterable(stream) {
				const reader = stream.getReader();
				try {
					while (true) {
						const { done, value } = await reader.read();
						if (done) {
							return;
						}
						yield value;
					}
				}
				finally {
					reader.releaseLock();
				}
			},
			waitingKeypress() {
				return new Promise((resolve) => {
					const handleKeyDown = (e) => {
						if (e.key == " ") {
							document.removeEventListener("keydown", handleKeyDown);
							resolve();
						}
					};
					document.addEventListener("keydown", handleKeyDown);
				});
			},
			async execute() {
				if (this.completed) {
					return;
				}
				switch (this.action.type) {
				case "file.create":
					await this.executeFileCreate(...this.action.args);
					break;
				case "file.write":
					await this.executeFileWrite(...this.action.args);
					break;
				case "file.select":
					await this.executeFileSelect(...this.action.args);
					break;
				case "wait":
					await this.sleep(parseInt(this.action.args[0]) * 1000);
					break;
				case "exec":
					{
						const index = this.terminal.length;
						this.$set(this.terminal, index, "\x1b[0;33mbzd\x1b[0m:\x1b[34m~/" + this.scenario.name + "\x1b[0m$ ");
						await this.emulateTypingTerminal((this.action.args[1] || this.action.args[0]) + "\n");
						const stream = await this.$api.request("post", "/exec", {
							cmds: this.action.args[0].split(" "),
						});
						for await (const chunk of this.streamAsyncIterable(stream)) {
							const blob = new Blob([chunk.buffer], { type: "text/plain; charset=utf-8" });
							const text = await blob.text();
							this.terminal.push(text);
						}
					}
					break;
				}

				if (!this.fastforward) {
					await this.waitingKeypress();
				}

				++this.index;
				this.$routerDispatch("/" + this.index);
			},
			handleTerminalProcessed(count) {
				this.terminal.splice(0, count);
			},
			async handleTreeSelected(path) {
				await this.executeFileSelect(path);
			},
		},
	};
</script>

<style lang="scss">
	* {
		box-sizing: border-box;
	}
	html,
	body {
		height: 100%;
		width: 100%;
		padding: 0;
		margin: 0;
		font-family: monospace;
	}
	.content {
		code {
			padding-left: 3ch;
			display: inline-block;
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

	@keyframes selection-blink {
		50% {
			opacity: 0;
		}
	}

	.selection {
		&:after {
			content: " ";
			background: #999;
			display: inline-block;
			vertical-align: bottom;
			animation: selection-blink 1s steps(1) infinite;
		}
	}
</style>

<style lang="scss" scoped>
	.layout {
		height: 100%;
		width: 100%;
		padding: 0;
		margin: 0;

		display: grid;
		grid-gap: 0;
		grid-template-rows: 1fr auto;
		grid-template-columns: auto 1fr;
		grid-template-areas:
			"tree content"
			"terminal terminal";

		> * {
			min-width: 0;
			min-height: 0;
			margin: 0;
			padding: 15px;
			overflow: auto;
		}

		.tree {
			grid-area: tree;

			padding: 10px 25px;
			border-right: 1px solid #ddd;
		}
		.content {
			grid-area: content;

			padding: 10px;
			white-space: pre-wrap;
		}
		.terminal {
			grid-area: terminal;
			border-top: 1px solid #ddd;
		}
	}
</style>
