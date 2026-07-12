<template>
	<div class="value-container" @click.stop="">
		<a v-if="enableViewAll" class="action" @click="setViewAll" v-tooltip="tooltipActionMore">+</a>
		<a v-else-if="enableViewOriginal" class="action" @click="setViewOriginal" v-tooltip="tooltipActionLess">-</a>
		<div class="value-grid" @click.stop="onClick">
			<div v-for="[t, v, isValid, metadata] in valueDisplay" :class="classItem(isValid)">
				<span class="timestamp">{{ timestampToString(t) }}</span
				><span class="value">
					<code class="json">{{ JSON.stringify(v) }}</code>
				</span>
				<span class="metadata">
					{{ metadataToString(metadata, v) }}
				</span>
			</div>
		</div>
	</div>
</template>

<script>
	import { dateToString, bytesToString, timeToString } from "#bzd/nodejs/utils/to_string.js";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.js";
	import Component from "#bzd/nodejs/vue/components/layout/component.vue";
	import ExceptionFactory from "#bzd/nodejs/core/exception.js";

	const Exception = ExceptionFactory("db", "data");

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
			timestampToString(timestamp) {
				return dateToString("{y:04}-{m:02}-{d:02} {h:02}:{min:02}:{s:02}", timestamp);
			},
			onClick() {
				this.$emit("select");
			},
			metadataToString(metadata, value) {
				const formatterFromUnit = (unit) => {
					if (unit === undefined) {
						return (_) => undefined;
					}
					let formatter = (value) => JSON.stringify(value) + metadata.unit;
					switch (metadata.unit) {
						case "bytes":
							formatter = bytesToString;
							break;
						case "seconds":
							formatter = timeToString;
							break;
					}
					// There is a unit, the data must be a number then.
					return (value) => {
						if (typeof value !== "number") {
							return undefined;
						}
						return formatter(value);
					};
				};
				const formatter = formatterFromUnit(metadata.unit);

				let entries = [];
				const formattedValue = formatter(value);
				if (formattedValue) {
					entries.push(formattedValue);
				}
				for (const [key, value] of Object.entries(metadata)) {
					if (key == "unit") {
						continue;
					}
					const valueStr = ["max", "min", "avg", "mean"].includes(key)
						? (formatter(value) ?? JSON.stringify(value))
						: JSON.stringify(value);
					entries.push(key + ": " + valueStr);
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
