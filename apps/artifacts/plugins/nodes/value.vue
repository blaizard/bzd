<template>
	<div class="value-container">
		<template v-if="isObject"
			>{<br />
			<div v-for="(subValue, key) in value" class="indent">
				<span>{{ key }}</span
				>: <Value :value="subValue" :path-list="pathList + [key]"></Value>,
			</div>
			}</template
		>
		<template v-else-if="isArray"
			>[
			<div v-for="subValue in value" class="inline"><Value :value="subValue" :path-list="pathList"></Value>,</div>
			]</template
		>
		<template v-else>
			<div class="inline">
				{{ valueToString(value) }}
			</div>
		</template>
	</div>
</template>

<script>
	export default {
		name: "Value",
		props: {
			value: { mandatory: false, type: Object },
			pathList: { mandatory: false, type: Array },
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
		},
		methods: {
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
	.value-container {
		display: inline;

		.inline {
			display: inline;
		}

		.indent {
			padding-left: 20px;
		}
	}
</style>
