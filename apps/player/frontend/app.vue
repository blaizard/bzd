<template>
	<div class="layout" :style="styleLayout">
		<!--<div :class="setComponentClass('tree')">
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
			<Terminal ref="terminal1"></Terminal>
		</div>
		<div :class="setComponentClass('terminal2')" @click="selectComponents('terminal2')">
			<Terminal ref="terminal2"></Terminal>
		</div>//-->
		<!--<Camera class="camera"></Camera>//-->
		<div
			v-for="(component, id) in layoutComponents"
			:key="id"
			:class="setComponentClass(id, component.type)"
			:style="{ 'grid-area': id }"
			@click="selectComponents(id)">
			<Terminal v-if="component.type == 'terminal'" :ref="id"></Terminal>
			<Editor
				v-else-if="component.type == 'editor' && selectedFile"
				:value="content"
				@input="handleContentUpdate"
				:path="selectedFile.path"
				:caret="cursor"
				:focus="selectedComponents.includes('content')"></Editor>
			<Tree
				v-else-if="component.type == 'tree'"
				:node="root"
				:selected="files.selected"
				@selected="handleTreeSelected"></Tree>
			<div class="name" v-if="component.name">{{ component.name }}</div>
		</div>
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
	import ExceptionFactory from "bzd/core/exception.mjs";

	const Log = LogFactory("app");
	const Exception = ExceptionFactory("app");

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
				layoutComponents: {
					"tree-0": { type: "tree" },
					"editor-0": { type: "editor" },
					"terminal-0": { type: "terminal" },
				},
				layout: [
					["tree-0", "editor-0"],
					["terminal-0", "terminal-0"],
				],
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
			styleLayout() {
				const areas = this.layout.map((line) => "\"" + line.join(" ") + "\"").join("\n");
				return {
					"grid-template-areas": areas,
					"grid-template-rows": this.layout.map(() => "1fr").join(" "),
					"grid-template-columns": this.layout[0].map(() => "1fr").join(" "),
				};
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
			selectComponents(...names) {
				this.selectedComponents = [...names];
			},
			setComponentClass(name, type) {
				return {
					[type]: true,
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
			async emulateTypingTerminal(terminal, value) {
				for (const c of value) {
					await this.emulateTypingKey(c);
					await this.$refs[terminal][0].setInput(c);
				}
			},
			async emulateTypingKey() {
				if (!this.fastforward) {
					await this.sleep(10 + Math.random() * 200);
				}
			},
			async executeFileCreate(path, executable = false) {
				this.selectedComponents = ["tree"];
				await this.files.createFile(path, "");
				await this.emulateTyping(this.files.selected.node, "name", FileSystem.basename(path));
				if (executable !== false) {
					await this.files.setExecutable(path);
				}
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
			async executeExec(cmdStr, terminal = "terminal-0") {
				this.selectedComponents = [terminal];
				await this.emulateTypingTerminal(terminal, cmdStr + "\n");
				// const stream = await this.$api.request("post", "/exec", {
				// cmds: cmdStr.split(" "),
				// });
				// // Refresh the view every 1s.
				// const refresher = setInterval(() => {
				// this.files.refresh();
				// }, 1000);
				// try {
				// for await (const chunk of streamAsyncIterable(stream)) {
				// const blob = new Blob([chunk.buffer], { type: "text/plain; charset=utf-8" });
				// const text = await blob.text();
				// this.terminal.push(text);
				// }
				// this.files.refresh();
				// }
				// finally {
				// clearInterval(refresher);
				// this.setPrompt();
				// }
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
			async executeConfigLayout(...lines) {
				let layout = [];
				let layoutComponents = {};
				const availableTypes = {
					terminal: Terminal,
					editor: Editor,
					tree: Tree,
				};
				for (const line of lines) {
					layout.push(
						line
							.split(" ")
							.filter(Boolean)
							.map((item) => {
								const data = item.split(":");
								const type = data[0];
								Exception.assert(type in availableTypes, "Unsupported type '{}'", data[0]);

								// Generate a unique ID or use the one passed into argument.
								let id = data[1];
								let counter = 0;
								while (id === undefined) {
									id = type + "-" + counter++;
									if (id in layoutComponents) {
										id = undefined;
									}
								}
								// Register the component.
								layoutComponents[id] = {
									type: type,
									name: null,
								};
								return id;
							})
					);
				}
				this.layout = layout;
				this.layoutComponents = layoutComponents;
			},
			async executeConfigName(id, name) {
				Exception.assert(id in this.layoutComponents, "Id '{}' is not a valid identifier.", id);
				this.layoutComponents[id].name = name;
			},
			async execute() {
				if (this.completed) {
					return;
				}

				if (!this.fastforward) {
					await this.waitingKeypress();
				}

				switch (this.action.type) {
				case "config.layout":
					await this.executeConfigLayout(...this.action.args);
					break;
				case "config.name":
					await this.executeConfigName(...this.action.args);
					break;
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

	$selectionWidth: 3;

	.layout {
		max-height: 100%;
		max-width: 100%;
		aspect-ratio: 16/9;
		padding: 0;
		border: #{$selectionWidth}px solid #000;
		background-color: white;

		// Center the layout in the screen.
		position: fixed;
		margin: auto;
		top: 0;
		bottom: 0;
		left: 0;
		right: 0;

		display: grid;
		grid-gap: #{$selectionWidth}px;
		//grid-template-rows: 1fr auto;
		//grid-template-columns: auto 1fr;
		//grid-template-rows: 1fr;
		//grid-template-columns: 1fr 1fr;

		> * {
			min-width: 0;
			min-height: 0;
			margin: 0;
			overflow: auto;
			position: relative;
			transition: max-height 1s ease-out;
		}

		> .selected {
			outline: #{$selectionWidth}px solid colors.$bzdGraphColorBlue !important;
			z-index: 1;
		}

		.name {
			position: absolute;
			top: 0;
			right: 0;
			background: rgba(255, 255, 255, 0.8);
			padding: 0 5px;
		}
		.tree {
			padding: 10px 25px;
			outline: 1px solid #ddd;
		}
		.editor {
			padding: 10px;
			white-space: pre-wrap;
		}
		.terminal {
			padding: 0;
			overflow: hidden;

			/*max-height: 50vh;
			&.selected {
				max-height: 50vh;
			}*/
		}
		.camera {
			position: absolute;
			bottom: 20px;
			right: 20px;
			width: 14%;
			aspect-ratio: 1;
			border-radius: 50%;
			overflow: hidden;
			z-index: 3;
		}
	}
</style>
