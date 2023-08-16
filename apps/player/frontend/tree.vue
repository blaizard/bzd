<template>
	<div class="container">
		<div v-if="empty" class="entry child">[empty]</div>
		<template v-else v-for="node in orderedList">
			<div :key="node.name" :class="getClass(node)" @click="handleClick(node)">
				<i v-if="node.isFolder()" class="bzd-icon-folder"></i>
				<span class="text">{{ node.name }}</span>
			</div>
			<div v-if="node.expanded">
				<TreeDirectory
					:node="node"
					:depth="depth + 1"
					:selected="selected"
					class="indent"
					@selected="$emit('selected', $event)"
				></TreeDirectory>
			</div>
		</template>
	</div>
</template>

<script>
	import Component from "bzd/vue/components/layout/component.vue";

	export default {
		mixins: [Component],
		name: "TreeDirectory",
		props: {
			node: { type: Object, mandatory: true },
			selected: { type: Object, mandatory: false, default: null },
			depth: { type: Number, mandatory: false, default: 0 },
		},
		data: function () {
			return {
				list: [],
			};
		},
		watch: {
			node: {
				async handler() {
					let list = [];
					for await (const node of this.node.get()) {
						list.push(node);
					}
					this.list = list;
				},
				immediate: true,
				deep: true,
			},
		},
		computed: {
			empty() {
				return this.orderedList.length === 0;
			},
			orderedList() {
				this.list.sort((a, b) => {
					if (a.isFolder() != b.isFolder()) {
						if (a.isFolder()) {
							return -1;
						}
						return 1;
					}
					return Intl.Collator().compare(a.name, b.name);
				});
				return this.list;
			},
		},
		methods: {
			getClass(node) {
				return {
					entry: true,
					child: this.depth > 0,
					expandable: node.isFolder(),
					expanded: node.expanded,
					selected: this.isSelected(node),
					clickable: this.isClickable(node),
				};
			},
			isSelected(node) {
				return this.selected && node.node === this.selected.node;
			},
			isClickable(node) {
				return "path" in node.node;
			},
			handleClick(node) {
				if (this.isClickable(node)) {
					this.$emit("selected", node.node.path);
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

			&.selected > .text {
				background-color: colors.$bzdGraphColorBlue;
				color: #fff;
				padding: 1px 5px;
			}
		}
	}
</style>
