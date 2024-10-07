<template>
	<div class="value-container">
		<div v-for="[t, v] in valueTimestampList" class="inline">
			<span class="timestamp">{{ timestampToString(t) }}</span
			><span class="value"><slot :value="v"></slot></span>
		</div>
	</div>
</template>

<script>
	import { dateToString } from "#bzd/nodejs/utils/to_string.mjs";

	export default {
		props: {
			value: { mandatory: true },
			pathList: { mandatory: true, type: Array },
		},
		computed: {
			valueTimestampList() {
				return [this.value];
			},
		},
		methods: {
			timestampToString(timestamp) {
				return dateToString("{y}-{m}-{d} {h}:{min}:{s}", timestamp);
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

		.timestamp {
			background-color: #eee;
			border: solid 1px rgb(229.5, 229.5, 229.5);
			border-radius: 6px;
			padding: 2px 5px;
			font-size: 70%;
			margin-right: 5px;
			opacity: 70%;
		}

		.value {
		}
	}
</style>
