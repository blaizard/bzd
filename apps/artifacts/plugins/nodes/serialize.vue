<template>
	<div class="serialize-container">
		<template v-if="isObject"
			>{<br />
			<div v-for="(key, index) in Object.keys(value).sort()" class="indent">
				<span>{{ key }}</span
				>:
				<Serialize :value="value[key]" :path-list="pathListAppendKey(key)" v-slot="slotProps">
					<slot :value="slotProps.value" :path-list="slotProps.pathList"></slot>
				</Serialize>
				<span v-if="index != Object.keys(value).length - 1">, </span>
			</div>
			}</template
		>
		<template v-else-if="isArray">
			<template v-if="isPathList">
				<slot :value="value" :path-list="pathList"></slot>
			</template>
			<template v-else>
				[
				<div v-for="(subValue, index) in value" class="inline">
					<Serialize :value="subValue" :path-list="pathList" v-slot="slotProps">
						<slot :value="slotProps.value" :path-list="slotProps.pathList"></slot>
					</Serialize>
					<span v-if="index != value.length - 1">, </span>
				</div>
				]</template
			>
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
			isObject() {
				return !!this.value && this.value.constructor === Object;
			},
			isArray() {
				return !!this.value && this.value.constructor === Array;
			},
			isString() {
				return typeof this.value === "string";
			},
			isNull() {
				return this.value === null;
			},
			isPathList() {
				return this.pathList !== null;
			},
		},
		methods: {
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
	.serialize-container {
		display: inline;

		.inline {
			display: inline;
		}

		.indent {
			padding-left: 20px;
		}
	}
</style>
