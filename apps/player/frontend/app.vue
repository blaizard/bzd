<template>
	<div class="layout">
		<div class="tree">
			<Tree :list="tree"></Tree>
		</div>
		<div class="content">
			<code v-if="files.selected" v-html="contentHTML"></code>
		</div>
	</div>
</template>

<script>
	import Layout from "bzd/vue/components/layout/layout.vue";
	import Tree from "./tree.vue";
	import Scenario from "../lib/scenario.mjs";
	import FileSystem from "../lib/filesystem.mjs";
	import "highlight.js/styles/github.css";
	import HighlightJs from "highlight.js/lib/common";

	export default {
		components: {
			Layout,
			Tree,
		},
		data: function () {
			return {
				start: 0,
				index: 0,
				treeRawData: {},
				scenario: new Scenario(),
				files: new FileSystem(),
				file: null,
				contentHightlighted: "",
				selection: [0, 0],
			};
		},
		watch: {
			content: {
				handler() {
					const first = this.content.slice(0, this.selection[0]);
					const selected = this.content.slice(this.selection[0], this.selection[1]);
					const last = this.content.slice(this.selection[1]);
					const hightlightedFirst = HighlightJs.highlight(first, { language: "cc" }).value;
					const hightlightedLast = HighlightJs.highlight(last, { language: "cc" }).value;
					this.contentHightlighted =
						hightlightedFirst + "<span class=\"selection\">" + selected + "</span>" + hightlightedLast;
				},
			},
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
			this.treeRawData["t"] = "";
		},
		methods: {
			sleep(ms) {
				if (!this.fastforward) {
					return new Promise((resolve) => setTimeout(resolve, ms));
				}
			},
			async emulateTyping(object, keyName, value) {
				for (const c of value) {
					await this.sleep(10 + Math.random() * 200);
					object[keyName] += c;
					this.selection = [object[keyName].length, object[keyName].length];
				}
			},
			async executeFileCreate(path) {
				await this.files.createFile(path);
				const name = this.files.selected.node.name;
				this.files.selected.node.name = "";
				await this.emulateTyping(this.files.selected.node, "name", name);
			},
			async executeFileWrite(path, content) {
				if (!this.files.select(path)) {
					await this.executeFileCreate(path);
				}
				await this.files.selected.updateContent(async () => {
					await this.emulateTyping(this.files.selected, "content", content);
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
				}

				this.next();
			},
			next() {
				++this.index;
				this.$routerDispatch("/" + this.index);
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
		padding: 15px;
		display: flex;
		flex-flow: row wrap;
		> * {
			height: 100%;
			overflow: auto;
		}
		.tree {
			max-width: 50%;
			margin: 0;
			padding: 10px;
			padding-right: 25px;
			border-right: 1px solid #ddd;
		}
		.content {
			flex: 1;
			padding: 10px;
			white-space: pre-wrap;
			margin-left: 15px;
		}
	}
</style>
