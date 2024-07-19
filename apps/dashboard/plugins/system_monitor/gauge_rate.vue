<template>
	<div v-if="nbObjects > 0">
		<template v-if="nbObjects <= max">
			<div class="gauge" v-for="(data, inputName) in input">
				<div class="name">
					<span class="small">{{ name.substring(0, 3) }}</span
					>.{{ inputName.toUpperCase() }}
				</div>
				<div class="values">
					<div class="value">
						<div class="bar fit-2" :style="gaugeStyle(sanitize(data).in)"></div>
						<div class="bar fit-2" :style="gaugeStyle(sanitize(data).out)"></div>
						<div class="overlay">{{ bytesToRateString(sanitize(data).in + sanitize(data).out) }}</div>
					</div>
				</div>
			</div>
		</template>
		<template v-else>
			<div class="gauge">
				<div class="name">{{ name }}</div>
				<div class="values">
					<div class="value">
						<div class="bar" :style="gaugeStyle(aggregatedRate.in)"></div>
						<div class="bar" :style="gaugeStyle(aggregatedRate.out)"></div>
						<div class="overlay">{{ bytesToRateString(aggregatedRate.in + aggregatedRate.out) }}</div>
					</div>
				</div>
			</div>
		</template>
	</div>
</template>

<script>
	import Gauge from "#bzd/apps/dashboard/plugins/system_monitor/gauge.vue";
	import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
	import { bytesToString } from "#bzd/nodejs/utils/to_string.mjs";

	const Exception = ExceptionFactory("plugin", "system_monitor", "gauge_rate");

	export default {
		extends: Gauge,
		props: {
			ratio: { type: Number, default: 20000 },
		},
		computed: {
			aggregatedRate() {
				return this.sanitize(Object.values(this.input).map(this.sanitize));
			},
		},
		methods: {
			// Take a list or an object and returns an object.
			//
			// It supports the following:
			// - {in: 0, out: 2}
			// - [{in: 0, out: 2}, {in: 0, out: 2}]
			// - [{in: 0, out: 2}, {in: 0, out: 2}]
			sanitize(rateOrRates) {
				if (!Array.isArray(rateOrRates)) {
					return {
						in: rateOrRates.in,
						out: rateOrRates.out,
					};
				}

				switch (this.policy) {
					case "sum":
						const [sumIn, sumOut] = rateOrRates.reduce((obj, data) => [obj[0] + data.in, obj[1] + data.out], [0, 0]);
						return {
							in: sumIn,
							out: sumOut,
						};
					default:
						Exception.error("Unsupported policy '{}'.", this.policy);
				}
			},
			gaugeStyle(data) {
				const x = data / this.ratio;
				// From a positive value, return a rate from 0 to 1.
				const percent = -1 / Math.log(x + Math.exp(1)) + 1;
				return {
					width: percent * 100 + "%",
				};
			},
			bytesToRateString(data) {
				return bytesToString(data) + "/s";
			},
		},
	};
</script>
