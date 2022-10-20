<template>
	<div class="layout">
		<div class="tree">
			<Tree :list="tree"></Tree>
		</div>
		<div class="content"></div>
	</div>
</template>

<script>
	import Layout from "bzd/vue/components/layout/layout.vue";
	import Tree from "./tree.vue";
	import Scenario from "../lib/scenario.mjs";
	import FileSystem from "../lib/filesystem.mjs";

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
						let file = await this.files.createFile(this.action.args[0]);
						file.name = "";
						await this.emulateTyping(file, "name", this.action.args[0]);
					}
					break;
				case "file.write":
					{
						// Look for the file.
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
			border: 1px solid #ddd;
			height: 100%;
			overflow: auto;
		}
		.tree {
			width: 25%;
			max-width: 400px;
			margin-right: 15px;
			padding: 10px;
		}
		.content {
			flex: 1;
		}
	}
</style>
