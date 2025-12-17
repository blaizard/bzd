<template>
	<div class="file-explorer">
		<div class="toolbar">
			<div class="tool" v-tooltip="{ type: 'text', data: 'Refresh' }" @click="refresh">
				<i class="bzd-icon-refresh"></i>
			</div>
		</div>
		<Tree
			:key="refreshCounter"
			:fetch="fetch"
			:show-path="path"
			@item="handleEventPropagation('item', $event)"
			:metadata="metadata"
		></Tree>
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
			path: { type: Array, mandatory: false, default: () => [] },
			fetch: { type: Function, mandatory: true },
			metadata: { type: Boolean, default: false },
		},
		data: function () {
			return {
				refreshCounter: 0,
			};
		},
		emits: ["item"],
		methods: {
			handleEventPropagation(name, event) {
				this.$emit(name, event);
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
	}
</style>
