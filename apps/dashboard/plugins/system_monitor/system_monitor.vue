<template>
	<div class="system-monitor">
		<!-- Header //-->
		<div class="header">
			<span
				v-if="!isMapEmpty(metadata.temperature || {})"
				class="entry"
				v-tooltip="makeTooltipMulti('Max Temperature', temperatures.tooltips)"
			>
				<i class="bzd-icon-thermometer"></i>
				<span class="value">{{ temperatures.max }}°C</span>
			</span>

			<span
				v-if="!isMapEmpty(metadata.battery || {})"
				class="entry"
				v-tooltip="makeTooltipMulti('Battery', batteries.tooltips)"
			>
				<i class="bzd-icon-battery"></i>
				<span class="value">{{ (batteries.min * 100).toFixed(1) }}%</span>
			</span>

			<span v-if="uptime" class="entry" v-tooltip="uptime.tooltip">
				<i class="bzd-icon-clock"></i>
				<span class="value">{{ uptime.text }}</span>
			</span>
		</div>

		<!-- Gauges //-->
		<div class="gauges">
			<Gauge
				name="CPU"
				:input="cpus.ratios"
				policy="average"
				v-tooltip="makeTooltipMulti('CPU', cpus.tooltips)"
			></Gauge>
			<Gauge
				name="GPU"
				:input="gpus.ratios"
				policy="average"
				v-tooltip="makeTooltipMulti('GPU', gpus.tooltips)"
			></Gauge>
			<Gauge
				name="MEMORY"
				:input="memories.ratios"
				policy="average"
				v-tooltip="makeTooltipMulti('Memory', memories.tooltips)"
			></Gauge>
			<Gauge
				name="DISK"
				:input="disks.ratios"
				policy="average"
				v-tooltip="makeTooltipMulti('Disk', disks.tooltips)"
			></Gauge>
			<GaugeRate name="IO" :input="ios.rates" policy="sum" v-tooltip="makeTooltipMulti('IO', ios.tooltips)"></GaugeRate>
			<GaugeRate
				name="NETWORK"
				:input="networks.rates"
				policy="sum"
				v-tooltip="makeTooltipMulti('Network', networks.tooltips)"
			></GaugeRate>
			<Text name="VERSION" :input="versions.values" v-tooltip="makeTooltipMulti('Version', versions.tooltips)"></Text>
		</div>
	</div>
</template>

<script>
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import { bytesToString, timeMsToString, capitalize } from "#bzd/nodejs/utils/to_string.mjs";
	import Gauge from "#bzd/apps/dashboard/plugins/system_monitor/gauge.vue";
	import GaugeRate from "#bzd/apps/dashboard/plugins/system_monitor/gauge_rate.vue";
	import Text from "#bzd/apps/dashboard/plugins/system_monitor/text.vue";

	export default {
		props: {
			metadata: { type: Object, mandatory: true },
		},
		components: {
			Gauge,
			GaugeRate,
			Text,
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		data: function () {
			return {
				ratesIO: {},
				ratesNetwork: {},
			};
		},
		watch: {
			"metadata.active": {
				handler(value) {
					this.$emit("active", value);
				},
			},
		},
		computed: {
			// Memory
			memories() {
				let ratios = {};
				let tooltips = {};
				for (const [name, memory] of Object.entries(this.metadata.memory || {})) {
					const { used, total } = memory;
					tooltips[name] =
						bytesToString(used) + " used / " + bytesToString(total) + " (" + ((used / total) * 100).toFixed(1) + "%)";
					ratios[name] = [
						{
							ratio: used / total,
							weight: used,
						},
					];
				}
				return {
					ratios,
					tooltips,
				};
			},
			disks() {
				let ratios = {};
				let tooltips = {};
				for (const [name, disk] of Object.entries(this.metadata.disk || {})) {
					const { used, total } = disk;
					tooltips[name] =
						bytesToString(used) + " used / " + bytesToString(total) + " (" + ((used / total) * 100).toFixed(1) + "%)";
					ratios[name] = [
						{
							ratio: used / total,
							weight: used,
						},
					];
				}
				return {
					ratios,
					tooltips,
				};
			},
			cpus() {
				let ratios = {};
				let tooltips = {};
				for (const [name, data] of Object.entries(this.metadata.cpu || {})) {
					tooltips[name] = (this.avgArray(data) * 100).toFixed(1) + "% load";
					ratios[name] = data;
				}
				return {
					ratios,
					tooltips,
				};
			},
			gpus() {
				let ratios = {};
				let tooltips = {};
				for (const [name, data] of Object.entries(this.metadata.gpu || {})) {
					tooltips[name] = (this.avgArray(data) * 100).toFixed(1) + "% load";
					ratios[name] = data;
				}
				return {
					ratios,
					tooltips,
				};
			},
			ios() {
				let tooltips = {};
				for (const [name, io] of Object.entries(this.metadata.io || {})) {
					const rates = this.updateInOutCountersToRates(this.ratesIO, name, io.in, io.out);
					tooltips[name] = "read: " + this.formatBytesRate(rates.in) + ", write: " + this.formatBytesRate(rates.out);
				}
				return {
					rates: this.ratesIO,
					tooltips,
				};
			},
			networks() {
				let rates = {};
				let tooltips = {};
				for (const [name, network] of Object.entries(this.metadata.network || {})) {
					const rates = this.updateInOutCountersToRates(this.ratesNetwork, name, network.in, network.out);
					tooltips[name] = "recv: " + this.formatBytesRate(rates.in) + ", send: " + this.formatBytesRate(rates.out);
				}
				return {
					rates: this.ratesNetwork,
					tooltips,
				};
			},
			versions() {
				let values = {};
				let tooltips = {};
				for (const [name, version] of Object.entries(this.metadata.version || {})) {
					values[name] = version;
					tooltips[name] = version;
				}
				return {
					values: values,
					tooltips: tooltips,
				};
			},
			temperatures() {
				let max = -Infinity;
				let tooltips = {};
				for (const [name, data] of Object.entries(this.metadata.temperature || {})) {
					const temperatureMax = this.maxArray(data);
					tooltips[name] = temperatureMax.toFixed(1) + "°C";
					max = Math.max(max, temperatureMax);
				}
				return {
					max,
					tooltips,
				};
			},
			batteries() {
				let min = Infinity;
				let tooltips = {};
				for (const [name, data] of Object.entries(this.metadata.battery || {})) {
					const batteryMin = this.minArray(data);
					tooltips[name] = (batteryMin * 100).toFixed(1) + "%";
					min = Math.min(min, batteryMin);
				}
				return {
					min,
					tooltips,
				};
			},
			uptime() {
				if (this.metadata.uptime) {
					const text = timeMsToString(this.metadata.uptime * 1000);
					return {
						text: text,
						tooltip: { data: "This node is/was up for " + text + " (" + parseInt(this.metadata.uptime) + "s)" },
					};
				}
				return null;
			},
		},
		methods: {
			// Utilities
			isMapEmpty(map) {
				return Object.keys(map).length === 0;
			},
			maxArray(array) {
				return array.reduce((m, v) => Math.max(m, v), -Infinity);
			},
			minArray(array) {
				return array.reduce((m, v) => Math.min(m, v), Infinity);
			},
			sumArray(array) {
				return array.reduce((a, b) => a + b, 0);
			},
			avgArray(array) {
				return this.sumArray(array) / array.length;
			},
			formatBytesRate(rate) {
				return bytesToString(rate) + "/s";
			},
			makeTooltipMulti(displayName, map) {
				const messageList = Object.entries(map).map(([name, data]) => {
					return '<li style="white-space: nowrap;">' + name + ": " + data + "</li>";
				});
				return { data: displayName + "<ul>" + messageList.join("\n") + "</ul>" };
			},
			updateInOutCountersToRates(map, name, counterIn, counterOut) {
				const timestampMs = Date.now();
				let rateIn = 0;
				let rateOut = 0;
				if (name in map) {
					const previousCounterIn = map[name].counterIn;
					const previousCounterOut = map[name].counterOut;
					const previousTimestampMs = map[name].timestampMs;
					rateIn = ((counterIn - previousCounterIn) / (timestampMs - previousTimestampMs)) * 1000;
					rateOut = ((counterOut - previousCounterOut) / (timestampMs - previousTimestampMs)) * 1000;
				}
				map[name] = {
					counterIn,
					counterOut,
					timestampMs,
					in: rateIn,
					out: rateOut,
				};
				return map[name];
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/icons.scss" as * with (
		$bzdIconNames: thermometer battery clock
	);

	.system-monitor {
		.header {
			text-align: right;
			margin-bottom: 20px;
			.entry {
				margin-left: 20px;
				.value {
					padding-left: 3px;
				}
			}
		}

		.gauges {
			width: 100%;
			padding-right: 10px;
		}
	}
</style>
