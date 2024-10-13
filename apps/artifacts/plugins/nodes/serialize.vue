<template>
	<div class="serialize-container">
		<template v-if="isValue">
			<slot :value="value" :path-list="pathList.slice(0, -1)"></slot>
		</template>
		<template v-else-if="isObject">
			<div v-for="key in Object.keys(value).sort()" class="indent">
				<a v-if="key != '_'" :href="keyToUrl(key)">{{ key }}</a>
				<Serialize class="child" :value="value[key]" :path-list="pathListAppendKey(key)" v-slot="slotProps">
					<slot :value="slotProps.value" :path-list="slotProps.pathList"></slot>
				</Serialize>
			</div>
		</template>
		<template v-else-if="isArray">
			[
			<div v-for="(subValue, index) in value" class="inline">
				<Serialize :value="subValue" :path-list="pathList" v-slot="slotProps">
					<slot :value="slotProps.value" :path-list="slotProps.pathList"></slot>
				</Serialize>
				<span v-if="index != value.length - 1">, </span>
			</div>
			]
		</template>
		<template v-else>
			<div class="inline">
				{{ valueToString(value) }}
			</div>
		</template>
	</div>
</template>

<script>
	export default {
		name: "Serialize",
		props: {
			value: { mandatory: true },
			pathList: { mandatory: false, default: null, type: Array || null },
		},
		computed: {
			isValue() {
				return this.pathList && this.pathList[this.pathList.length - 1] == "_";
			},
			isObject() {
				return !!this.value && this.value.constructor === Object;
			},
			isArray() {
				return !!this.value && this.value.constructor === Array;
			},
		},
		methods: {
			keyToUrl(key) {
				return "/view/" + [...this.pathList, key].map(encodeURIComponent).join("/");
			},
			pathListAppendKey(key) {
				if (this.pathList === null) {
					return null;
				}
				return [...this.pathList, key];
			},
			valueToString(value) {
				if (typeof value === "string") {
					return '"' + value + '"';
				}
				if (value === null) {
					return "null";
				}
				return value.toString();
			},
		},
	};
</script>

<style lang="scss" scoped>
	@use "#bzd/config.scss" as config;

	$indent: 20;

	.serialize-container {
		display: inline;

		.inline {
			display: inline;
		}

		.indent {
			padding-left: #{$indent}px;
			position: relative;
			border-left: 1px dotted config.$bzdGraphColorGray;

			&:not(:hover) > a {
				color: config.$captionTextColor;
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
