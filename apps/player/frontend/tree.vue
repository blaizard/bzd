<template>
	<div class="container">
		<template v-for="node in list">
			<div :key="node.name" :class="getClass(node)" @click="handleClick(node)">
				<i v-if="isFolder(node)" class="bzd-icon-folder"></i>
				{{ node.name }}
			</div>
			<div v-if="isExpandedFolder(node)">
				<TreeDirectory
					:tree="node.children"
					:depth="depth + 1"
					:selected="selected"
					class="indent"
					@selected="$emit('selected', $event)"></TreeDirectory>
			</div>
		</template>
	</div>
</template>

<script>
	import Component from "bzd/vue/components/layout/component.vue";
	import FileSystem from "../lib/filesystem.mjs";

	export default {
		mixins: [Component],
		name: "TreeDirectory",
		props: {
			tree: { type: Object, mandatory: false, default: () => [] },
			selected: { type: Object, mandatory: false, default: null },
			depth: { type: Number, mandatory: false, default: 0 },
		},
		data: function () {
			return {};
		},
		computed: {
			list() {
				let list = Object.values(this.tree);
				list.sort((a, b) => Intl.Collator().compare(a.name, b.name));
				return list;
			},
		},
		methods: {
			getClass(node) {
				return {
					entry: true,
					child: this.depth > 0,
					expandable: this.isFolder(node),
					expanded: this.isExpandedFolder(node),
					selected: this.isSelected(node),
					clickable: this.isClickable(node),
				};
			},
			isFolder(node) {
				return FileSystem.isFolder(node);
			},
			isSelected(node) {
				return this.selected && node === this.selected.node;
			},
			isExpandedFolder(node) {
				return node.expanded || false;
			},
			isClickable(node) {
				return "path" in node;
			},
			handleClick(node) {
				if (this.isClickable(node)) {
					this.$emit("selected", node.path);
				}
			},
		},
	};
</script>

<style lang="scss">
	@use "bzd/icons.scss" as icons with (
		$bzdIconNames: folder
	);
</style>

<style lang="scss" scoped>
	@use "sass:math";
	@use "bzd-style/css/colors.scss" as colors;

	$indent: 20;
	$arrowSize: 5;
	$fontSize: 16;
	$lineHeight: 24;
	$arrowOffsetY: math.div($lineHeight, 2);
	$lineColor: colors.$bzdGraphColorGray;

	.indent {
		padding-left: #{$indent}px;
		position: relative;
		&:after {
			position: absolute;
			content: "";
			border-left: 1px dotted $lineColor;
			left: 0;
			top: 0;
			bottom: #{$lineHeight - $arrowOffsetY - 1}px;
		}
	}

	.container {
		font-size: #{$fontSize}px;
		line-height: #{$lineHeight}px;

		.error {
			color: colors.$bzdGraphColorRed;
			white-space: nowrap;
			overflow: hidden;
			text-overflow: ellipsis;
			&:hover {
				white-space: pre-wrap;
			}
		}

		.entry {
			position: relative;
			padding-left: #{$arrowSize * 2}px;
			white-space: nowrap;
			user-select: none;

			&.clickable {
				cursor: pointer;
			}

			&.child:after {
				position: absolute;
				content: "";
				border-top: 1px dotted $lineColor;
				left: #{-$indent + 1}px;
				top: #{$arrowOffsetY}px;
				width: #{$indent - $arrowSize * 2}px;
			}

			&.expandable {
				&:before {
					position: absolute;
					left: #{math.div(-$arrowSize, 2)}px;
					top: #{$arrowOffsetY - $arrowSize}px;
					width: 0;
					height: 0;
					border-left: #{$arrowSize}px solid colors.$bzdGraphColorBlack;
					border-right: #{$arrowSize}px solid transparent;
					border-bottom: #{$arrowSize}px solid transparent;
					border-top: #{$arrowSize}px solid transparent;
					content: "";
					transition: transform 0.5s;
				}
			}

			&.expanded:before {
				transform: rotate(90deg) translateY(#{math.div($arrowSize, 2)}px) translateX(#{math.div($arrowSize, 2)}px);
			}

			&.selected {
				background-color: colors.$bzdGraphColorBlue;
				color: #fff;
			}
		}
	}
</style>
