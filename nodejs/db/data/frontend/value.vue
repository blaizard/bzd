<template>
	<div class="value-container" @click.stop="">
		<a v-if="enableViewAll" class="action" @click="setViewAll" v-tooltip="tooltipActionMore">+</a>
		<a v-else-if="enableViewOriginal" class="action" @click="setViewOriginal" v-tooltip="tooltipActionLess">-</a>
		<div class="value-grid" @click.stop="onClick">
			<div v-for="([t, v], index) in valueDisplay" :class="classItem(!isExpired(t))">
				<span class="timestamp">{{ timestampToString(t) }}</span
				><span class="value">
					<code class="json">{{ JSON.stringify(v) }}</code>
				</span>
				<span class="metadata">
					{{ metadataToString(v, index == 0) }}
				</span>
			</div>
		</div>
	</div>
</template>

<script>
	import { dateToString, timeToString, UCUMToString } from "#bzd/nodejs/utils/to_string.js";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.js";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import ExceptionFactory from "#bzd/nodejs/core/exception.js";

	const Exception = ExceptionFactory("db", "data");

	export default {
		mixins: [Component],
		props: {
			value: { mandatory: true, type: Array },
			view: { default: 1, type: Number },
			timestamp: { mandatory: true, type: Number },
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
			metadata() {
				// The most recent entry get the metadata
				if (this.value) {
					return {
						expires: this.value[0][2],
						unit: this.value[0][3],
					};
				}
				return {};
			},
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
				return this.nbDisplay == 0 ? this.value : this.value.slice(0, this.nbDisplay);
			},
		},
		methods: {
			classItem(isValid) {
				return {
					item: true,
					"item-expired": !isValid,
				};
			},
			setViewAll() {
				this.viewAll = true;
			},
			setViewOriginal() {
				this.viewAll = false;
			},
			timestampToString(timestampMs) {
				return dateToString("{y:04}-{m:02}-{d:02} {h:02}:{min:02}:{s:02}", timestampMs);
			},
			onClick() {
				this.$emit("select");
			},
			isExpired(timestampMs) {
				if (this.metadata.expires) {
					return this.timestamp > timestampMs + this.metadata.expires * 1000;
				}
				return false;
			},
			valueFormatted(value, unit) {
				const processIfTypeof = (expectedTypeof, callable, value, postfix) => {
					if (typeof value === expectedTypeof) {
						return callable(value) + postfix;
					}
					return undefined;
				};
				const formatter = (value) => {
					if (!unit || typeof value != "number") {
						return undefined;
					}
					return UCUMToString(value, unit);
				};
				let entries = [];
				if (Array.isArray(value)) {
					if (value.length == 1 && unit) {
						return [formatter(value[0]) ?? unit];
					} else if (value.length > 1) {
						try {
							const min = Math.min(...value);
							if (min !== NaN) {
								const max = Math.max(...value);
								const mean = value.reduce((acc, val) => acc + val, 0) / value.length;
								Exception.assert();
								return [
									"mean: " + (formatter(mean) ?? mean),
									"min: " + (formatter(min) ?? min),
									"max: " + (formatter(max) ?? max),
								];
							}
						} catch (e) {
							// ignore.
						}
					}
				} else if (unit) {
					return [formatter(value) ?? unit];
				}
				return [];
			},
			metadataToString(value, isNewest) {
				let entries = [...this.valueFormatted(value, this.metadata.unit)];
				if (isNewest) {
					if (this.metadata.expires) {
						entries.push("expires: " + timeToString(this.metadata.expires));
					}
				}
				return entries.join(", ");
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

				&.item-expired {
					opacity: 0.5;
				}

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

				.metadata {
					align-self: baseline;
					white-space: pre;
					opacity: 0.5;
					font-size: 0.8em;
					padding: 2px 5px;
				}
			}
		}
	}
</style>
