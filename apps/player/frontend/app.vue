<template>
	<div class="layout">
		<div :class="setComponentClass('tree')">
			<Tree :node="root" :selected="files.selected" @selected="handleTreeSelected"></Tree>
		</div>
		<div :class="setComponentClass('content')" @click="selectComponents('content')">
			<Editor
				v-if="selectedFile"
				:value="content"
				@input="handleContentUpdate"
				:path="selectedFile.path"
				:caret="cursor"
				:focus="selectedComponents.includes('content')"></Editor>
		</div>
		<div :class="setComponentClass('terminal1')" @click="selectComponents('terminal1')">
			<Terminal :stream="terminal" @processed="handleTerminalProcessed"></Terminal>
		</div>
		<Camera class="camera"></Camera>
	</div>
</template>

<script>
	import Layout from "bzd/vue/components/layout/layout.vue";
	import Tree from "./tree.vue";
	import Terminal from "./terminal.vue";
	import Editor from "./editor.vue";
	import Camera from "./camera.vue";
	import Scenario from "../lib/scenario.mjs";
	import FileSystem from "../lib/filesystem.mjs";
	import LogFactory from "bzd/core/log.mjs";

	const Log = LogFactory("app");

	export default {
		components: {
			Layout,
			Tree,
			Terminal,
			Editor,
			Camera,
		},
		data: function () {
			return {
				start: 0,
				index: 0,
				scenario: new Scenario(),
				files: new FileSystem(this.$api),
				selectedComponents: [],
				cursor: 0,
				terminal: [],
			};
		},
		computed: {
			root() {
				return this.files.makeRootNode(this.scenario.name);
			},
			completed() {
				return this.index >= this.scenario.size;
			},
			action() {
				return this.scenario.at(this.index);
			},
			fastforward() {
				return this.index < this.start;
			},
			selectedFile() {
				if (this.files.selected && this.files.selected.isFile()) {
					return this.files.selected;
				}
				return null;
			},
			content() {
				if (this.selectedFile) {
					return this.selectedFile.content;
				}
				return "";
			},
		},
		async mounted() {
			this.scenario = new Scenario(await this.$api.request("get", "/scenario"));
			this.setPrompt();
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
				this.cursor = this.content.length;
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
			async emulateTypingKey() {
				if (!this.fastforward) {
					await this.sleep(10 + Math.random() * 200);
				}
			},
			async executeFileCreate(path) {
				this.selectedComponents = ["tree"];
				await this.files.createFile(path, "");
				await this.emulateTyping(this.files.selected.node, "name", FileSystem.basename(path));
			},
			async executeFileWrite(path, content) {
				if (!(await this.files.select(path))) {
					await this.executeFileCreate(path);
				}
				this.selectedComponents = ["content"];
				await this.files.selected.updateContent(async () => {
					await this.emulateTyping(this.files.selected, "content", content);
				});
			},
			async executeFileSelect(path) {
				this.selectedComponents = ["tree"];
				const node = await this.files.select(path);
				if (node.isFile()) {
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

				this.selectedComponents = ["terminal1"];
				await this.emulateTypingTerminal((cmdStrDisplay || cmdStr) + "\n");
				const stream = await this.$api.request("post", "/exec", {
					cmds: cmdStr.split(" "),
				});
				// Refresh the view every 1s.
				const refresher = setInterval(() => {
					this.files.refresh();
				}, 1000);
				try {
					for await (const chunk of streamAsyncIterable(stream)) {
						const blob = new Blob([chunk.buffer], { type: "text/plain; charset=utf-8" });
						const text = await blob.text();
						this.terminal.push(text);
					}
					this.files.refresh();
				}
				finally {
					clearInterval(refresher);
					this.setPrompt();
				}
			},
			/// Set the prompt of the terminal.
			setPrompt() {
				this.$set(
					this.terminal,
					this.terminal.length,
					"\x1b[0;33mbzd\x1b[0m:\x1b[34m~/" + this.scenario.name + "\x1b[0m$ "
				);
			},
			waitingKeypress() {
				return new Promise((resolve) => {
					const handleKeyDown = (e) => {
						if (e.key == "Escape") {
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
			async handleContentUpdate(data) {
				let file = await this.files.select(data.path, /*select*/ false);
				await file.saveContent(data.content);
				if (this.files.selected.path == data.path) {
					this.files.selected.content = data.content;
				}
				Log.info("Auto-saved {}", file.path);
			},
		},
	};
</script>

<style lang="scss">
	@use "bzd-style/css/colors.scss" as colors;

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
		background-color: black;
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

	::selection {
		color: #fff;
		background-color: colors.$bzdGraphColorBlue;
	}
</style>

<style lang="scss" scoped>
	@use "bzd-style/css/colors.scss" as colors;

	.layout {
		max-height: 100%;
		max-width: 100%;
		aspect-ratio: 16/9;
		padding: 0;
		border: 1px solid #000;
		background-color: white;

		// Center the layout in the screen.
		position: fixed;
		margin: auto;
		top: 0;
		bottom: 0;
		left: 0;
		right: 0;

		display: grid;
		grid-gap: 0;
		grid-template-rows: 1fr auto;
		grid-template-columns: auto 1fr;
		grid-template-areas:
			"tree content"
			"terminal1 terminal1";

		> * {
			min-width: 0;
			min-height: 0;
			margin: 0;
			overflow: auto;
		}

		> .selected {
			outline: 2px solid colors.$bzdGraphColorBlue !important;
			z-index: 1;
		}

		> div {
			transition: max-height 1s ease-out;
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
		.terminal1 {
			grid-area: terminal1;
			padding: 0;
			overflow: hidden;

			max-height: 50vh;
			&.selected {
				max-height: 80vh;
			}
		}
		.camera {
			position: absolute;
			bottom: 20px;
			right: 20px;
			width: 14%;
			aspect-ratio: 1;
			border-radius: 50%;
			overflow: hidden;
			z-index: 2;
		}
	}
</style>
