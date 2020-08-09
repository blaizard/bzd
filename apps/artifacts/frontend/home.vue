<template>
	<div class="container">
		<template v-for="item in list">
			<div :key="item.name" @click="handleClick(item)" :class="getClass(item)">
				{{ item.name }}
			</div>
			<div v-if="item.name in expanded" :key="item.name + '.expanded'">
				<TreeDirectory :path="makePath(item)" :depth="depth + 1" class="indent"></TreeDirectory>
			</div>
		</template>
		<div v-if="isEmpty">&lt;emtpy&gt;</div>
	</div>
</template>

<script>
	export default {
		name: "TreeDirectory",
		props: {
			path: { type: String, mandatory: false, default: "/" },
			depth: { type: Number, mandatory: false, default: 0 },
		},
		data: function () {
			return {
				next: true,
				list: [],
				expanded: {},
			};
		},
		mounted() {
			this.fetchPath();
		},
		computed: {
			isEmpty() {
				return this.list.length == 0 && this.next === null;
			},
		},
		methods: {
			async fetchPath() {
				if (this.next === null) {
					return;
				}
				const response = await this.$api.request("get", "/list", {
					path: this.path,
					paging: this.next === true ? 1 : JSON.stringify(this.next),
				});
				this.next = response.next;
				this.list = this.list.concat(response.data);
				this.list.sort(new Intl.Collator().compare);
				if (this.next) {
					this.fetchPath();
				}
			},
			getClass(item) {
				return {
					entry: true,
					child: (this.depth > 0),
					expandable: this.isExpandable(item),
					expanded: item.name in this.expanded,
				};
			},
			isExpandable(item) {
				return ["directory", "bucket"].includes(item.type);
			},
			makePath(item) {
				return this.path + ("path" in item ? item.path : item.name) + "/";
			},
			handleClick(item) {
				if (!this.isExpandable(item)) {
					return;
				}
				const name = item.name;
				if (name in this.expanded) {
					this.$delete(this.expanded, name);
				}
				else {
					this.$set(this.expanded, name, true);
				}
			},
		},
	};
</script>

<style lang="scss" scoped>
	@use "bzd-style/css/colors.scss" as colors;

	$indent: 20;
	$arrowSize: 5;
	$lineHeight: 20;
	$arrowOffsetY: $lineHeight / 2;
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

		line-height: #{$lineHeight}px;

		.entry {
			position: relative;
			padding-left: #{$arrowSize * 2}px;

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
					left: #{-$arrowSize / 2}px;
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
				transform: rotate(90deg) translateY(#{$arrowSize / 2}px) translateX(#{$arrowSize / 2}px);
			}
		}
	}
</style>
