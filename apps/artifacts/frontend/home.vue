<template>
	<div class="container">
		<template v-for="item in list">
			<div :key="item.name" @click="handleClick(item)" :class="getClass(item)">
				{{ item.name }}
			</div>
			<div v-if="item.type == 'directory' && item.name in expanded" :key="item.name + '.expanded'">
				<TreeDirectory :path="makePath(item)" class="indent"></TreeDirectory>
			</div>
		</template>
	</div>
</template>

<script>
	export default {
		name: "TreeDirectory",
		props: {
			path: { type: String, mandatory: false, default: "/" },
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
		methods: {
			async fetchPath() {
				if (this.next === null) {
					return;
				}
				const response = await this.$api.request("get", "/list", {
					path: this.path,
					paging: this.next === true ? 10 : JSON.stringify(this.next),
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
					"arrow-down": item.type == "directory",
					"arrow-left": item.name in this.expanded,
				};
			},
			makePath(item) {
				return this.path + item.name + "/";
			},
			handleClick(item) {
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

	$indent: 20px;

	.indent {
		padding-left: $indent;
	}

	.container {
		position: relative;
		&:after {
			position: absolute;
			content: "";
			border-left: 1px dotted colors.$bzdGraphColorBlack;
			left: 0;
			top: 0;
			bottom: 1ex;
		}

		.entry {
			position: relative;
			padding-left: 20px;

			&:after {
				position: absolute;
				content: "";
				border-top: 1px dotted colors.$bzdGraphColorBlack;
				left: -$indent;
				top: 1ex;
				width: $indent;
			}

			&.arrow-down:before {
				position: absolute;
				left: 0px;
				top: 1ex;
				width: 0;
				height: 0;
				border-left: 5px solid transparent;
				border-right: 5px solid transparent;
				border-bottom: 5px solid transparent;
				border-top: 5px solid colors.$bzdGraphColorBlack;
				content: "";
				transition: transform 0.5s;
			}

			&.arrow-left:before {
				transform: rotate(-90deg) translateY(2.5px) translateX(2.5px);
			}
		}
	}
</style>
