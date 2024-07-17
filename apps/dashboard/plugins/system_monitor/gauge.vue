<template>
	<div>
		<template v-if="Object.keys(ratios).length <= max" v-for="(ratio, name) in ratios">
			<div class="gauge">
				<div class="name">{{ name.toUpperCase() }}</div>
				<div class="values">
					<div class="value">
						<div class="bar" :style="gaugeStyle(sanitize(ratio))"></div>
						<div class="overlay">{{ (sanitize(ratio) * 100).toFixed(1) }}%</div>
					</div>
				</div>
			</div>
		</template>
		<template v-else>
			<div class="gauge">
				<div class="name">{{ name }}</div>
				<div class="values">
					<div class="value">
						<div class="bar" :style="gaugeStyle(ratio)"></div>
						<div class="overlay">{{ (ratio * 100).toFixed(1) }}%</div>
					</div>
				</div>
			</div>
		</template>
	</div>
</template>

<script>
	import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

	const Exception = ExceptionFactory("plugin", "system_monitor");

	export default {
		props: {
			ratios: { type: Object, mandatory: true },
			name: { type: String, mandatory: true },
			policy: { type: String, mandatory: true },
			max: { type: Number, default: 2 },
		},
		computed: {
			// Convert list of ratios into one.
			sanitizedRatios() {},
			ratio() {
				return this.sanitize(Object.values(this.ratios).map(this.sanitize));
			},
		},
		methods: {
			// Take a list or a number and returns a number.
			sanitize(ratiosOrRatio) {
				if (typeof ratiosOrRatio == "number") {
					return ratiosOrRatio;
				}
				switch (this.policy) {
					case "average":
						const ratioSum = ratiosOrRatio.reduce((ratioSum, ratio) => ratioSum + ratio, 0);
						return ratioSum / ratiosOrRatio.length;
						break;
					case "max":
						return ratiosOrRatio.reduce(Math.max, 0);
					default:
						Exception.error("Unsupported policy '{}'.", this.policy);
				}
			},
			gaugeStyle(ratio) {
				return {
					width: ratio * 100 + "%",
				};
			},
		},
	};
</script>

<style lang="scss">
	.gauge {
		width: 100%;
		display: flex;
		flex-direction: row;
		flex-wrap: nowrap;
		font-size: 0.8em;
		margin-bottom: 5px;

		.name {
			width: 30%;
			text-align: right;
			padding-right: 10px;
		}
		.values {
			flex: 1;
			.value {
				overflow: hidden;
				white-space: nowrap;
				position: relative;
				isolation: isolate;
				height: 1.5em;
				line-height: 1.5em;

				&:before {
					content: " ";
					position: absolute;
					left: 0;
					top: 0;
					bottom: 0;
					right: 0;
					background-color: currentColor;
					opacity: 0.1;
				}

				&:after {
					content: " ";
				}

				.bar {
					position: absolute;
					top: 0;
					left: 0;
					bottom: 0;
					background-color: currentColor;
					transition: width 0.5s;
					mix-blend-mode: difference;

					&.fit-2 {
						position: relative;
						height: calc(50% - 1px);
						margin: 1px 0;
					}
				}

				.overlay {
					position: absolute;
					top: 0;
					width: 100%;
					mix-blend-mode: difference;
					text-align: right;
					padding: 0 4px;
				}
			}
		}
	}
</style>
