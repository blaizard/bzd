<template>
	<div v-if="nbObjects > 0">
		<template v-if="nbObjects <= max">
			<div class="gauge" v-for="(ratio, inputName) in input">
				<div class="name">
					<span class="small">{{ name.substring(0, 3) }}.</span>{{ inputName.toUpperCase() }}
				</div>
				<div class="values">
					<div class="value">
						<div class="bar" :style="gaugeStyle(sanitize(ratio).ratio)"></div>
						<div class="overlay">{{ (sanitize(ratio).ratio * 100).toFixed(1) }}%</div>
					</div>
				</div>
			</div>
		</template>
		<template v-else>
			<div class="gauge">
				<div class="name">{{ name }}</div>
				<div class="values">
					<div class="value">
						<div class="bar" :style="gaugeStyle(aggregatedRatio)"></div>
						<div class="overlay">{{ (aggregatedRatio * 100).toFixed(1) }}%</div>
					</div>
				</div>
			</div>
		</template>
	</div>
</template>

<script>
	import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

	const Exception = ExceptionFactory("plugin", "system_monitor", "gauge");

	export default {
		props: {
			input: { type: Object, mandatory: true },
			name: { type: String, mandatory: true },
			policy: { type: String, mandatory: true },
			max: { type: Number, default: 2 },
		},
		computed: {
			nbObjects() {
				return Object.keys(this.input).length;
			},
			aggregatedRatio() {
				return this.sanitize(Object.values(this.input).map(this.sanitize)).ratio;
			},
		},
		methods: {
			// Take a list or a number and returns an object.
			//
			// It supports the following:
			// - 0.5
			// - [0.2, 0.3, 0.6]
			// - [{ratio:0.2}, {ratio:0.5}]
			// - [{ratio:0.2, weight:23}, {ratio:0.5, weight:65}]
			sanitize(ratiosOrRatio) {
				// Helper function.
				const getWeightedRatio = (obj) => {
					if (typeof obj == "number") {
						return [obj, 0];
					}
					if (obj.weight) {
						return [obj.ratio * obj.weight, obj.weight];
					}
					return [obj.ratio, 0];
				};

				if (typeof ratiosOrRatio == "number") {
					return {
						ratio: ratiosOrRatio,
					};
				}

				switch (this.policy) {
					case "average":
						const [ratioSum, weights] = ratiosOrRatio.reduce(
							(obj, data) => {
								const [weightedRatio, weight] = getWeightedRatio(data);
								return [obj[0] + weightedRatio, obj[1] + weight];
							},
							[0, 0],
						);
						if (weights == 0) {
							return {
								ratio: ratioSum / ratiosOrRatio.length,
							};
						}
						return {
							ratio: ratioSum / weights,
							weight: weights,
						};
					case "max":
						return {
							ratio: ratiosOrRatio.reduce(Math.max, 0),
						};
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
			overflow: hidden;
			height: 1.5em;

			.small {
				font-size: 0.6em;
			}
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
