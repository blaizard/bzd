<template>
	<div class="layout">
		<div class="tree">
			<Tree :list="tree"></Tree>
		</div>
		<div class="content">
			<code v-if="file" v-html="highlightedContent"></code>
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
				highlightedContent: "",
			};
		},
		watch: {
			file: {
				handler() {
					const html = HighlightJs.highlight(this.file.content, { language: "cc" }).value;
					this.highlightedContent = html
						.split("\n")
						.map((line, index) => "<span class=\"line-number\">" + index + "</span>" + line)
						.join("\n");
				},
				deep: true,
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
					await this.sleep(100);
					object[keyName] += c;
				}
			},
			async execute() {
				if (this.completed) {
					return;
				}
				switch (this.action.type) {
				case "file.create":
					{
						this.file = await this.files.createFile(this.action.args[0]);
						this.file.name = "";
						await this.emulateTyping(this.file, "name", this.action.args[0]);
					}
					break;
				case "file.write":
					{
						this.file = this.files.get(this.action.args[0]);
						if (this.file === null) {
							this.file = await this.files.createFile(this.action.args[0]);
						}
						await this.emulateTyping(this.file, "content", this.action.args[1]);
					}
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
	.bzd-content {
		margin: 10px;
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
			width: 25%;
			max-width: 400px;
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

<style>
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
</style>
