<template>
	<div class="container" v-loading="loading">
		<template v-for="item in list">
			<div :key="item.name" @click="handleClick(item)" :class="getClass(item)">
				{{ item.name }}
				<span class="actions" @click.stop="">
					<i v-if="depth == 0" class="bzd-icon-configuration" @click="handleConfig(item.name)"></i>
				</span>
			</div>
			<div v-if="item.name in expanded" :key="item.name + '.expanded'">
				<TreeDirectory
					:path="makePath(item)"
					:depth="depth + 1"
					class="indent"
					@item="handleItemPropagation(item.name, $event)"
				></TreeDirectory>
			</div>
		</template>
		<div v-if="isError" class="error">{{ error }}</div>
		<div v-if="loading" class="loading">&nbsp;</div>
		<div v-else-if="isEmpty">&lt;emtpy&gt;</div>
	</div>
</template>

<script>
	import Component from "bzd/vue/components/layout/component.vue";
	import DirectiveLoading from "bzd/vue/directives/loading.mjs";

	export default {
		mixins: [Component],
		name: "TreeDirectory",
		props: {
			path: { type: Array, mandatory: false, default: () => [] },
			depth: { type: Number, mandatory: false, default: 0 }
		},
		directives: {
			loading: DirectiveLoading
		},
		data: function() {
			return {
				list: [],
				expanded: {},
				error: null
			};
		},
		mounted() {
			this.fetchPath();
		},
		computed: {
			isError() {
				return this.error !== null;
			},
			isEmpty() {
				return !this.loading && this.list.length == 0;
			}
		},
		methods: {
			async fetchPath() {
				try {
					await this.handleSubmit(async () => {
						this.list = await this.$cache.get("list", ...this.path);
					}, /*throwOnError*/ true);
				} catch (e) {
					this.error = e;
				}
			},
			getClass(item) {
				return {
					entry: true,
					child: this.depth > 0,
					expandable: this.isPermissionList(item),
					expanded: item.name in this.expanded
				};
			},
			isPermissionList(item) {
				return item.permissions.isList();
			},
			makePath(item) {
				return this.path.concat([item.name]);
			},
			handleClick(item) {
				// Propagate the item message up with the arguments
				this.$emit("item", {
					item: item,
					path: []
				});

				if (!this.isPermissionList(item)) {
					return;
				}
				const name = item.name;
				if (name in this.expanded) {
					this.$delete(this.expanded, name);
				} else {
					this.$set(this.expanded, name, true);
				}
			},
			handleConfig(name) {
				this.$routerDispatch("/config/" + name);
			},
			handleItemPropagation(name, item) {
				item.path.unshift(name);
				this.$emit("item", item);
			}
		}
	};
</script>

<style lang="scss">
	@use "bzd/icons.scss" as icons with (
        $bzdIconNames: configuration
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
