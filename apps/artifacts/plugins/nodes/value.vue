<template>
	<div class="value-container">
		<a class="more" @click="dsd" v-tooltip="tooltipActionMore">...</a>
		<div class="value-grid">
			<div v-for="[t, v] in valueTimestampList" class="item">
				<span class="timestamp">{{ timestampToString(t) }}</span
				><span class="value"><slot :value="v"></slot></span>
			</div>
		</div>
	</div>
</template>

<script>
	import { dateToString } from "#bzd/nodejs/utils/to_string.mjs";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";

	export default {
		props: {
			value: { mandatory: true },
			pathList: { mandatory: true, type: Array },
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		computed: {
			valueTimestampList() {
				return [this.value];
			},
			tooltipActionMore() {
				return {
					type: "text",
					data: "View more...",
				};
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
		display: inline-flex;

		a.more {
			background-color: #eee;
			border: solid 1px rgb(229.5, 229.5, 229.5);
			border-radius: 6px;
			padding: 2px 5px;
			font-size: 70%;
			margin-right: 5px;
			font-weight: bold;
			align-self: baseline;
		}

		.value-grid {
			display: inline-grid;
			align-self: baseline;

			.item {
				display: flex;

				.timestamp {
					background-color: #eee;
					border: solid 1px rgb(229.5, 229.5, 229.5);
					border-radius: 6px;
					padding: 2px 5px;
					font-size: 70%;
					margin-right: 5px;
					opacity: 70%;
					align-self: baseline;
					white-space: nowrap;
				}

				.value {
					align-self: baseline;
				}
			}
		}
	}
</style>
