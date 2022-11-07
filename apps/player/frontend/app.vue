<template>
	<div class="layout">
		<div :class="setComponentClass('tree')">
			<Tree :tree="tree" :selected="files.selected" @selected="handleTreeSelected"></Tree>
		</div>
		<div :class="setComponentClass('content')" @click="selectComponents('content')">
			<code v-if="files.selected" v-html="contentHTML"></code>
		</div>
		<Terminal
			:class="setComponentClass('terminal')"
			:stream="terminal"
			@processed="handleTerminalProcessed"
			@click="selectComponents('terminal')"></Terminal>
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
				selectedComponents: [],
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
				return { [this.scenario.name]: { name: this.scenario.name, expanded: true, children: this.files.data } };
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

			await this.files.list("/");
		},
		methods: {
			selectComponents(...names) {
				this.selectedComponents = [...names];
			},
			setComponentClass(name) {
				return {
					[name]: true,
					selected: this.selectedComponents.includes(name),
				};
			},
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
				if (!this.fastforward) {
					let audio = new Audio(audioSrc);
					await this.sleep(10 + Math.random() * 200);
					audio.volume = (key.charCodeAt(0) % 10) / 10 + 0.1;
					audio.play();
				}
			},
			async executeFileCreate(path) {
				this.selectedComponents = ["tree"];
				await this.files.createFile(path, "");
				await this.emulateTyping(this.files.selected.node, "name", FileSystem.basename(path));
			},
			async executeFileWrite(path, content) {
				if (!this.files.select(path)) {
					await this.executeFileCreate(path);
				}
				this.selectedComponents = ["content"];
				await this.files.selected.updateContent(async () => {
					await this.emulateTyping(this.files.selected, "content", content);
				});
			},
			async executeFileSelect(path) {
				this.selectedComponents = ["tree"];
				const node = this.files.select(path);
				if (FileSystem.isFile(node)) {
					await this.files.selected.fetchContent();
					this.setSelectionEnd();
				}
				else {
					await this.files.selected.toggleExpand();
				}
			},
			async executeExec(cmdStr, cmdStrDisplay = null) {
				async function* streamAsyncIterable(stream) {
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
				}

				this.selectedComponents = ["terminal"];
				const index = this.terminal.length;
				this.$set(this.terminal, index, "\x1b[0;33mbzd\x1b[0m:\x1b[34m~/" + this.scenario.name + "\x1b[0m$ ");
				await this.emulateTypingTerminal((cmdStrDisplay || cmdStr) + "\n");
				const stream = await this.$api.request("post", "/exec", {
					cmds: cmdStr.split(" "),
				});
				for await (const chunk of streamAsyncIterable(stream)) {
					const blob = new Blob([chunk.buffer], { type: "text/plain; charset=utf-8" });
					const text = await blob.text();
					this.terminal.push(text);
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

				if (!this.fastforward) {
					await this.waitingKeypress();
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
					await this.executeExec(...this.action.args);
					break;
				default:
					console.error("Unsupported command: " + this.action.type);
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
		display: none;
	}

	.selected .selection {
		display: inline;
	}
</style>

<style lang="scss" scoped>
	@use "bzd-style/css/colors.scss" as colors;

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

		> .selected {
			border: 2px solid colors.$bzdGraphColorBlue !important;
		}
		> :not(.selected) {
			border: 2px solid transparent;
		}

		.tree {
			grid-area: tree;

			padding: 10px 25px;
			outline: 1px solid #ddd;
		}
		.content {
			grid-area: content;

			padding: 10px;
			white-space: pre-wrap;
		}
		.terminal {
			grid-area: terminal;
		}
	}
</style>
