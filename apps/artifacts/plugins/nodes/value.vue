<template>
	<div class="value-container">
		<a class="more" @click="viewMore" v-tooltip="tooltipActionMore">...</a>
		<div class="value-grid">
			<div v-for="[t, v] in valueDisplay" class="item">
				<span class="timestamp">{{ timestampToString(t) }}</span
				><span class="value"><slot :value="v"></slot></span>
			</div>
		</div>
	</div>
</template>

<script>
	import { dateToString } from "#bzd/nodejs/utils/to_string.mjs";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

	const Exception = ExceptionFactory("apps", "plugin", "nodes");

	export default {
		mixins: [Component],
		props: {
			value: { mandatory: true, type: Array },
			view: { default: 1, type: Number },
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		computed: {
			tooltipActionMore() {
				return {
					type: "text",
					data: "View more...",
				};
			},
			valueDisplay() {
				return this.view == 0 ? this.value : this.value.slice(-this.view);
			},
		},
		methods: {
			timestampToString(timestamp) {
				return dateToString("{y:04}-{m:02}-{d:02} {h:02}:{min:02}:{s:02}", timestamp);
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
					white-space: pre;
				}
			}
		}
	}
</style>
