<template>
	<div class="value-container" @click.stop="">
		<a v-if="enableViewAll" class="action" @click="setViewAll" v-tooltip="tooltipActionMore">+</a>
		<a v-else-if="enableViewOriginal" class="action" @click="setViewOriginal" v-tooltip="tooltipActionLess">-</a>
		<div class="value-grid" @click.stop="onClick">
			<div v-for="[t, v] in valueDisplay" class="item">
				<span class="timestamp">{{ timestampToString(t) }}</span
				><span class="value">
					<code class="json">{{ JSON.stringify(v) }}</code>
				</span>
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
		emits: ["select"],
		data: function () {
			return {
				viewAll: false,
			};
		},
		computed: {
			enableViewAll() {
				return this.nbDisplay !== 0 && this.value.length > this.nbDisplay;
			},
			enableViewOriginal() {
				return this.viewAll === true && this.view !== 0;
			},
			nbDisplay() {
				return this.viewAll ? 0 : this.view;
			},
			tooltipActionMore() {
				return {
					type: "text",
					data: "View all values captured since the page opened.",
				};
			},
			tooltipActionLess() {
				return {
					type: "text",
					data: "Reduce the view.",
				};
			},
			valueDisplay() {
				return this.nbDisplay == 0 ? this.value : this.value.slice(-this.nbDisplay);
			},
		},
		methods: {
			setViewAll() {
				this.viewAll = true;
			},
			setViewOriginal() {
				this.viewAll = false;
			},
			timestampToString(timestamp) {
				return dateToString("{y:04}-{m:02}-{d:02} {h:02}:{min:02}:{s:02}", timestamp);
			},
			onClick() {
				this.$emit("select");
			},
		},
	};
</script>

<style lang="scss" scoped>
	.value-container {
		display: inline-flex;

		a.action {
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

					.json {
						background-color: transparent;
					}
				}
			}
		}
	}
</style>
