<template>
	<div class="file-explorer">
		<div class="toolbar">
			<div class="tool" v-tooltip="{ type: 'text', data: 'Refresh' }" @click="refresh">
				<i class="bzd-icon-refresh"></i>
			</div>
		</div>
		<div class="tree">
			<Tree
				:key="refreshCounter"
				:fetch="fetch"
				:download="download"
				:show-path="path ?? currentPath"
				@item="handleItem"
				:metadata="metadata"
				:refreshPeriosS="refreshPeriosS"
			></Tree>
		</div>
	</div>
</template>

<script>
	import Tree from "#bzd/nodejs/vue/components/file_explorer/tree.vue";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";

	export default {
		components: {
			Tree,
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		props: {
			path: { type: Array, default: null },
			fetch: { type: Function, mandatory: true },
			download: { type: Function, default: null },
			metadata: { type: Boolean, default: false },
			refreshPeriosS: { type: Number, default: 0 },
		},
		data: function () {
			return {
				refreshCounter: 0,
				currentPath: [],
			};
		},
		emits: ["item"],
		methods: {
			handleItem(item) {
				this.currentPath = [...item.path, item.item.name];
				this.$emit("item", item);
			},
			refresh() {
				++this.refreshCounter;
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/icons.scss" as icons with (
		$bzdIconNames: refresh
	);
</style>

<style lang="scss" scoped>
	@use "@/nodejs/styles/default/css/clickable.scss" as *;

	.file-explorer {
		container-type: inline-size;
		padding: 10px;

		.toolbar {
			display: flex;
			border-bottom: 1px solid black;
			padding: 5px;
			margin-bottom: 5px;

			.tool {
				width: 1.5em;
				height: 1.5em;
				line-height: 1.5em;
				text-align: center;
				@extend %bzd-clickable;
			}
		}

		.tree {
			display: table;
		}
	}
</style>
