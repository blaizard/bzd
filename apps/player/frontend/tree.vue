<template>
	<div class="container">
		<template v-for="item in list">
			<div :key="item.name" :class="getClass(item)">
				<i v-if="isFolder(item)" class="bzd-icon-folder"></i>
				{{ item.name }}
			</div>
			<div v-if="hasChildren(item)">
				<TreeDirectory :list="item.children" :depth="depth + 1" class="indent"></TreeDirectory>
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
			list: { type: Array, mandatory: false, default: () => [] },
			depth: { type: Number, mandatory: false, default: 0 },
		},
		data: function () {
			return {};
		},
		methods: {
			getClass(item) {
				return {
					entry: true,
					child: this.depth > 0,
					expandable: this.isFolder(item),
				};
			},
			isFolder(item) {
				return item.type == "folder";
			},
			hasChildren(item) {
				return item.children && item.children.length > 0;
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

			.actions {
				float: right;
				opacity: 0.3;
				&:hover {
					opacity: 1;
				}
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
				cursor: pointer;

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
		}
	}
</style>
