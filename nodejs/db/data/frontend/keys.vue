<template>
	<div class="keys-container">
		<template v-if="isObject">
			<div v-for="key in keys" class="indent" @click.stop="onClick(key)">
				<span v-if="key != '_'">{{ key }}</span>
				<Keys
					class="child"
					:value="value[key]"
					:path-list="pathListAppendKey(key)"
					v-slot="slotProps"
					@select="propagateSelect"
				>
					<slot :value="slotProps.value" :path-list="slotProps.pathList" @select="propagateSelect"></slot>
				</Keys>
			</div>
		</template>
		<template v-else>
			<slot :value="value" :path-list="pathList.slice(0, -1)" @select="propagateSelect"></slot>
		</template>
	</div>
</template>

<script>
	export default {
		name: "Keys",
		props: {
			value: { mandatory: true },
			pathList: { mandatory: true, type: Array },
		},
		computed: {
			isObject() {
				return !!this.value && this.value.constructor === Object;
			},
			keys() {
				// Sort lexically, except '_' should always be first.
				return Object.keys(this.value).sort((a, b) => {
					if (a === "_") return -1;
					if (b === "_") return 1;
					return a.localeCompare(b);
				});
			},
		},
		methods: {
			pathListAppendKey(key) {
				return [...this.pathList, key];
			},
			onClick(key) {
				if (key == "_") {
					this.$emit("select", this.pathList);
				} else {
					this.$emit("select", [...this.pathList, key]);
				}
			},
			propagateSelect(e) {
				this.$emit("select", e);
			},
		},
	};
</script>

<style lang="scss" scoped>
	@use "@/config.scss" as config;

	$indent: 20;

	.keys-container {
		display: inline;

		.inline {
			display: inline;
		}

		.indent {
			padding-left: #{$indent}px;
			position: relative;
			border-left: 1px dotted config.$bzdGraphColorGray;
			cursor: pointer;

			&:hover > span {
				text-shadow: 1px 0 #000;
			}
		}

		.child:after {
			position: absolute;
			content: "";
			border-top: 1px dotted config.$bzdGraphColorGray;
			left: 0;
			top: 14px;
			width: 14px;
		}
	}
</style>
