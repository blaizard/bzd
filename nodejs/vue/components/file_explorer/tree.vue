<template>
	<div v-if="loading" v-loading="loading" class="entry">
		<div class="name attribute" :style="styleName">...</div>
	</div>
	<div v-else-if="isEmpty" class="entry">
		<div class="name attribute" :style="styleName"><span class="name-content">&lt;empty&gt;</span></div>
	</div>
	<template v-else v-for="item in list" :key="item.name">
		<div @click="isAuthorized(item) && handleSelect(item)" :class="getClass(item)">
			<div class="name attribute" :style="styleName">
				<span class="name-content">
					<i v-if="!isAuthorized(item)" class="bzd-icon-lock icon"></i>
					<i v-if="isPermissionList(item)" class="bzd-icon-folder icon"></i>
					<a
						v-if="Boolean(download) && !isPermissionList(item)"
						@click="download([...path, item.name])"
						v-tooltip="{ type: 'text', data: 'Download ' + item.name }"
					>
						{{ item["name"] }}
					</a>
					<template v-else>
						{{ item["name"] }}
					</template>
				</span>
			</div>
			<div v-for="key in innerItemListSortedKeys.slice(1)" v-if="!isPermissionList(item)" class="attribute">
				{{ key in item ? displayValue(key, item[key]) : "" }}
			</div>
		</div>
		<template v-if="item.name in expanded && isAuthorized(item)" :key="item.name + '.expanded'">
			<TreeDirectory
				:fetch="fetch"
				:path="makePath(item)"
				:depth="depth + 1"
				:showPath="showPath"
				:metadata="metadata"
				:download="download"
				@item="handleItemPropagation(item.name, $event)"
			></TreeDirectory>
		</template>
	</template>
</template>

<script>
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.mjs";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import { bytesToString } from "#bzd/nodejs/utils/to_string.mjs";

	export default {
		mixins: [Component],
		name: "TreeDirectory",
		props: {
			showPath: { type: Array, mandatory: false, default: () => [] },
			path: { type: Array, mandatory: false, default: () => [] },
			depth: { type: Number, mandatory: false, default: 0 },
			fetch: { type: Function, mandatory: true },
			metadata: { type: Boolean, default: false },
			download: { type: Function, default: null },
		},
		directives: {
			loading: DirectiveLoading,
			tooltip: DirectiveTooltip,
		},
		emits: ["item"],
		data: function () {
			return {
				list: [],
				expanded: {},
			};
		},
		mounted() {
			this.fetchPath();
		},
		watch: {
			showPath: {
				handler() {
					this.updateExpand();
				},
				immediate: true,
			},
		},
		computed: {
			isEmpty() {
				return !this.loading && this.list.length == 0;
			},
			nameToExpand() {
				if (this.showPath.join("/").startsWith(this.path.join("/"))) {
					return this.showPath[this.depth] || false;
				}
				return false;
			},
			/// Return the keys for the entries within the current list (directory).
			innerItemListSortedKeys() {
				const sortStrategy = (key1, key2) => {
					const firsts = ["name", "type", "size", "permissions", "modified", "created"];
					const pos1 = firsts.indexOf(key1);
					const pos2 = firsts.indexOf(key2);
					if (pos1 == -1 && pos2 == -1) {
						return key1.localeCompare(key2);
					}
					if (pos1 == -1) {
						return 1;
					}
					if (pos2 == -1) {
						return -1;
					}
					return pos1 < pos2 ? -1 : 1;
				};
				let keys = new Set();
				for (const item of this.list) {
					Object.keys(item).forEach((key) => {
						keys.add(key);
					});
				}
				const keyList = Array.from(keys);
				return keyList.sort(sortStrategy).slice(0, this.metadata ? 5 : 1);
			},
			styleName() {
				return {
					"padding-left": 20 * this.depth + "px",
				};
			},
		},
		methods: {
			displayValue(key, value) {
				switch (key) {
					case "size":
						return bytesToString(value);
					default:
						return value;
				}
			},
			async fetchPath() {
				await this.handleSubmit(async () => {
					this.list = await this.fetch(this.path);
					// Set directories first.
					this.list.sort((a, b) => {
						const isAList = this.isPermissionList(a);
						const isBList = this.isPermissionList(b);
						if (isAList != isBList) {
							if (isAList) {
								return -1;
							}
							return 1;
						}
						return Intl.Collator().compare(a.name, b.name);
					});
					this.updateExpand();
				});
			},
			getClass(item) {
				return {
					entry: true,
					metadata: this.metadata,
					child: this.depth > 0,
					expandable: this.isPermissionList(item),
					expanded: item.name in this.expanded,
				};
			},
			isPermissionList(item) {
				return item.permissions.isList();
			},
			isAuthorized(item) {
				if ("authorized" in item) {
					return item.authorized;
				}
				return true;
			},
			makePath(item) {
				return this.path.concat([item.name]);
			},
			updateExpand() {
				if (this.nameToExpand) {
					const maybeItem = this.nameToItem(this.nameToExpand);
					if (maybeItem && this.isPermissionList(maybeItem)) {
						this.expanded[this.nameToExpand] = true;
					}
				}
			},
			nameToItem(name) {
				const maybeItem = this.list.find((item) => item.name == name);
				return maybeItem || null;
			},
			handleSelect(item) {
				const name = item.name;
				if (name in this.expanded) {
					delete this.expanded[name];
				} else {
					this.$emit("item", {
						item: item,
						path: [],
					});
					if (this.isPermissionList(item)) {
						this.expanded[name] = true;
					}
				}
			},
			handleItemPropagation(name, item) {
				item.path.unshift(name);
				this.$emit("item", item);
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/icons.scss" as icons with (
		$bzdIconNames: configuration folder lock
	);
</style>

<style lang="scss" scoped>
	@use "sass:math";
	@use "@/config.scss" as config;

	$indent: 20;
	$arrowSize: 5;
	$fontSize: 16;
	$lineHeight: 24;
	$arrowOffsetY: math.div($lineHeight, 2);
	$lineColor: config.$bzdGraphColorGray;

	.entry {
		display: table-row;
		position: relative;

		.attribute {
			display: table-cell;
			white-space: nowrap;
			cursor: pointer;
			padding-right: 20px;
		}
		&:hover {
			.attribute {
				opacity: 0.7;
			}
		}

		.name-content {
			position: relative;
			margin-left: #{$arrowSize * 2}px;
			padding-left: #{$arrowSize * 2}px;

			.icon {
				margin-right: 5px;
			}
		}

		// Vertical line for child items.
		&.child {
			.name-content:after {
				position: absolute;
				content: "";
				border-left: 1px dotted $lineColor;
				border-bottom: 1px dotted $lineColor;
				left: #{-$indent + 1}px;
				top: 0;
				bottom: #{$lineHeight - $arrowOffsetY - 1}px;
				width: #{$indent - $arrowSize * 2}px;
			}
		}

		// Arrow for expandable items.
		&.expandable {
			.name-content {
				&:before {
					position: absolute;
					left: #{math.div(-$arrowSize, 2)}px;
					top: #{$arrowOffsetY - $arrowSize}px;
					width: 0;
					height: 0;
					border-left: #{$arrowSize}px solid config.$bzdGraphColorBlack;
					border-right: #{$arrowSize}px solid transparent;
					border-bottom: #{$arrowSize}px solid transparent;
					border-top: #{$arrowSize}px solid transparent;
					content: "";
					transition: transform 0.5s;
				}
			}
		}
		&.expanded {
			.name-content:before {
				transform: rotate(90deg) translateY(#{math.div($arrowSize, 2)}px) translateX(#{math.div($arrowSize, 2)}px);
			}
		}

		// Responsive attributes display (display max 5 elements).
		.attribute {
			width: 20%;
		}
		.attribute:nth-child(n + 6) {
			display: none;
		}
		@container (max-width: 999px) {
			// Print 4 elements.
			.attribute {
				width: 25%;
			}
			.attribute:nth-child(n + 5) {
				display: none;
			}
		}
		@container (max-width: 799px) {
			// Print 3 elements.
			.attribute {
				width: 33%;
			}
			.attribute:nth-child(n + 4) {
				display: none;
			}
		}
		@container (max-width: 599px) {
			// Print 2 elements.
			.attribute {
				width: 50%;
			}
			.attribute:nth-child(n + 3) {
				display: none;
			}
		}
		@container (max-width: 399px) {
			// Print 1 element.
			.attribute {
				width: 100%;
			}
			.attribute:nth-child(n + 2) {
				display: none;
			}
		}
	}
</style>
