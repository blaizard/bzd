<template>
	<div class="layout">
		<div class="tree">
			<Tree :list="tree"></Tree>
		</div>
		<div class="content">&nbsp;</div>
	</div>
</template>

<script>
	import Layout from "bzd/vue/components/layout/layout.vue";
	import Tree from "./tree.vue";

	export default {
		components: {
			Layout,
			Tree,
		},
		data: function () {
			return {
				start: 0,
				index: 0,
				scenario: null,
				tree: [
					// {name: "project", type: "folder", children: [
					// {name: "file.txt", type: "txt"}
					// ]},
					// {name: "image.jpg", type: "jpg"}
				],
			};
		},
		computed: {
			flow() {
				return this.scenario ? this.scenario.flow : [];
			},
			completed() {
				return this.index >= this.flow.length;
			},
			action() {
				return this.flow[this.index].action;
			},
			args() {
				return this.flow[this.index].args || [];
			},
			fastforward() {
				return this.index < this.start;
			},
		},
		async mounted() {
			this.scenario = await this.$api.request("get", "/scenario");
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

				switch (this.action) {
				case "file.create":
					{
						let newFile = {
							name: "",
							type: "file",
						};
						this.tree.push(newFile);
						await this.emulateTyping(newFile, "name", this.args[0]);
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
