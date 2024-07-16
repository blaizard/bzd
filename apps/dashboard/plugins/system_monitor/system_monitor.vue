<template>
	<div class="system-monitor">
		<!-- Header //-->
		<div class="header">
			<span v-if="isTemperature" class="entry" v-tooltip="temperatureTooltip">
				<i class="bzd-icon-thermometer"></i>
				<span class="value">{{ temperatureMax.toFixed(0) }}°C</span>
			</span>

			<span v-if="isBattery" class="entry" v-tooltip="batteryTooltip">
				<i class="bzd-icon-battery"></i>
				<span class="value">{{ batteryPercent.toFixed(0) }}%</span>
			</span>
		</div>

		<!-- Gauges //-->
		<div class="gauges">
			<div v-if="isCpu" class="gauge" v-tooltip="cpuTooltip">
				<div class="name">CPU</div>
				<div class="values">
					<div class="value">
						<div class="bar" :style="cpuStyle"></div>
						<div class="overlay">{{ cpuPercent.toFixed(1) }}%</div>
					</div>
				</div>
			</div>
			<div v-if="isGpu" class="gauge" v-tooltip="gpuTooltip">
				<div class="name">GPU</div>
				<div class="values">
					<div class="value">
						<div class="bar" :style="gpuStyle"></div>
						<div class="overlay">{{ gpuPercent.toFixed(1) }}%</div>
					</div>
				</div>
			</div>

			<!-- memory //-->
			<template v-if="Object.keys(memories).length < 3" v-for="(memory, name) in memories">
				<div class="gauge" v-tooltip="makeTooltipSingle('Memory', name, memory)">
					<div class="name">{{ name.toUpperCase() }}</div>
					<div class="values">
						<div class="value">
							<div class="bar" :style="gaugeStyle(memory.ratio)"></div>
							<div class="overlay">{{ (memory.ratio * 100).toFixed(1) }}%</div>
						</div>
					</div>
				</div>
			</template>
			<template v-else>
				<div class="gauge" v-tooltip="makeTooltipMulti('Memory', memories)">
					<div class="name">MEMORY</div>
					<div class="values">
						<div class="value">
							<div class="bar" :style="gaugeStyle(memoriesRatio)"></div>
							<div class="overlay">{{ (memoriesRatio * 100).toFixed(1) }}%</div>
						</div>
					</div>
				</div>
			</template>

			<div v-if="isIO" class="gauge" v-tooltip="ioTooltip">
				<div class="name">IO</div>
				<div class="values">
					<div class="value">
						<div class="bar fit-2" :style="ioReadStyle"></div>
						<div class="bar fit-2" :style="ioWriteStyle"></div>
						<div class="overlay">{{ formatBytesRate(ioRateRead + ioRateWrite) }}</div>
					</div>
				</div>
			</div>
			<div v-if="isNetwork" class="gauge" v-tooltip="networkTooltip">
				<div class="name">Network</div>
				<div class="values">
					<div class="value">
						<div class="bar fit-2" :style="networkReadStyle"></div>
						<div class="bar fit-2" :style="networkWriteStyle"></div>
						<div class="overlay">{{ formatBytesRate(networkRateRead + networkRateWrite) }}</div>
					</div>
				</div>
			</div>
		</div>
	</div>
</template>

<script>
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import { bytesToString, capitalize } from "#bzd/nodejs/utils/to_string.mjs";

	export default {
		props: {
			metadata: { type: Object, mandatory: true },
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		data: function () {
			return {
				history: {},
			};
		},
		computed: {
			// Memory
			memoryTypes() {
				return Object.keys(this.metadata.memory || {});
			},
			memories() {
				let memories = {};
				for (const [name, memory] of Object.entries(this.metadata.memory || {})) {
					const [used, total] = memory;
					const free = total - used;
					memories[name] = {
						ratio: used / total,
						display:
							bytesToString(free) + " free / " + bytesToString(total) + " (" + ((free / total) * 100).toFixed(1) + "%)",
					};
				}
				return memories;
			},
			memoriesRatio() {
				const values = Object.values(this.memories);
				const ratioSum = values.reduce((ratioSum, memory) => ratioSum + memory.ratio, 0);
				return ratioSum / values.length;
			},
			// CPU
			isCpu() {
				return "cpu" in this.metadata;
			},
			cpuPercent() {
				return this.makeCpuPercent(this.metadata.cpu);
			},
			cpuStyle() {
				return {
					width: this.cpuPercent + "%",
				};
			},
			cpuTooltip() {
				return this.makeCpuTooltip("CPU", this.metadata.cpu);
			},
			// GPU
			isGpu() {
				return "gpu" in this.metadata;
			},
			gpuPercent() {
				return this.makeCpuPercent(this.metadata.gpu);
			},
			gpuStyle() {
				return {
					width: this.gpuPercent + "%",
				};
			},
			gpuTooltip() {
				return this.makeCpuTooltip("GPU", this.metadata.gpu);
			},
			// Temperature
			isTemperature() {
				return false; //this.has("temperature");
			},
			temperature() {
				return this.getItems("temperature").reduce((obj, item) => {
					const id = item.id.substring("temperature".length + 1) || "undefined";
					obj[id] = Math.max(obj[id] || 0, item.value);
					return obj;
				}, {});
			},
			temperatureMax() {
				return Object.keys(this.temperature).reduce((value, key) => Math.max(value, this.temperature[key]), 0);
			},
			temperatureTooltip() {
				return this.makeTooltip("Max Temperature", this.temperature, (temperature) => temperature.toFixed(0) + "°C");
			},
			// Battery
			isBattery() {
				return false; //this.has("ups.charge");
			},
			batteryPercent() {
				return this.getItems("ups.charge").reduce((value, item) => Math.min(value, item.value), 1) * 100;
			},
			batteryTooltip() {
				if (!this.getItems("ups.name")) {
					return null;
				}
				return {
					data: this.getItems("ups.name")
						.map((item) => item.value)
						.join(", "),
				};
			},
			// IO
			isIO() {
				return false; //this.has("io.total.in") || this.has("io.total.out");
			},
			ioRate() {
				return this.getRate("io", this.makeIOMap("io"));
			},
			ioRateRead() {
				return Object.values(this.ioRate).reduce((sum, obj) => sum + (obj.in || 0), 0);
			},
			ioRateWrite() {
				return Object.values(this.ioRate).reduce((sum, obj) => sum + (obj.out || 0), 0);
			},
			ioReadStyle() {
				return {
					width: this.makeRate(this.ioRateRead / 20000) * 100 + "%",
				};
			},
			ioWriteStyle() {
				return {
					width: this.makeRate(this.ioRateWrite / 20000) * 100 + "%",
				};
			},
			ioTooltip() {
				return this.makeRateTooltip("IO", this.ioRate, { in: "read", out: "write" });
			},
			// Network
			isNetwork() {
				return false; //this.has("network.total.in") || this.has("network.total.out");
			},
			networkRate() {
				return this.getRate("network", this.makeIOMap("network"));
			},
			networkRateRead() {
				return Object.values(this.networkRate).reduce((sum, obj) => sum + (obj.in || 0), 0);
			},
			networkRateWrite() {
				return Object.values(this.networkRate).reduce((sum, obj) => sum + (obj.out || 0), 0);
			},
			networkReadStyle() {
				return {
					width: this.makeRate(this.networkRateRead / 20000) * 100 + "%",
				};
			},
			networkWriteStyle() {
				return {
					width: this.makeRate(this.networkRateWrite / 20000) * 100 + "%",
				};
			},
			networkTooltip() {
				return this.makeRateTooltip("Network", this.networkRate, { in: "recv", out: "sent" });
			},
		},
		methods: {
			// Utilities
			maxArray(array) {
				return array.reduce(Math.max, 0);
			},
			sumArray(array) {
				return array.reduce((a, b) => a + b, 0);
			},
			avgArray(array) {
				return this.sumArray(array) / array.length;
			},
			avgMapOfArrays(map) {
				const [sum, size] = Object.values(map).reduce(
					(value, array) => {
						value[0] += this.sumArray(array);
						value[1] += array.length;
						return value;
					},
					[0, 0],
				);
				return sum / size;
			},
			// Gauge
			gaugeStyle(ratio) {
				return {
					width: ratio * 100 + "%",
				};
			},
			// CPU
			makeCpuPercent(map) {
				return this.avgMapOfArrays(map) * 100;
			},
			makeCpuTooltip(displayName, map) {
				return this.makeTooltip(displayName, map, (array) => {
					return (this.avgArray(array) * 100).toFixed(1) + "% load";
				});
			},
			// Memory
			memoryTooltip(memory) {
				return this.makeMemoryTooltip("Memory", { [memory]: this.metadata.memory[memory] });
			},
			memoryPercent(memory) {
				const [used, total] = this.metadata.memory[memory];
				return (used / total) * 100;
			},
			memoryStyle(memory) {
				return {
					width: this.memoryPercent(memory) + "%",
				};
			},
			makeMemoryTooltip(displayName, map) {
				return this.makeTooltip(displayName, map, (item) => {
					const total = item[1];
					const free = total - item[0];
					return (
						bytesToString(free) + " free / " + bytesToString(total) + " (" + ((free / total) * 100).toFixed(1) + "%)"
					);
				});
			},
			// IO
			makeIOMap(name) {
				let map = this.getItems(name + ".total.in").reduce((obj, item) => {
					const id = item.id.substring((name + ".total.in").length + 1) || "undefined";
					obj[id] = obj[id] || { in: 0, out: 0 };
					obj[id].in += item.value;
					return obj;
				}, {});
				this.getItems(name + ".total.out").forEach((item) => {
					const id = item.id.substring((name + ".total.out").length + 1) || "undefined";
					map[id] = map[id] || { in: 0, out: 0 };
					map[id].out += item.value;
				});
				return map;
			},
			addHistory(name, map) {
				this.history[name] = this.history[name] || [];
				// Find the insertion timestamp of the last element
				let lastTimestamp = 0;
				if (this.history[name].length) {
					lastTimestamp = this.history[name][0].timestamp;
				}

				// Add a new element every 6 seconds
				if (Date.now() - lastTimestamp > 6000) {
					this.history[name].unshift({
						timestamp: Date.now(),
						value: JSON.parse(JSON.stringify(map)),
					});
					// Keep max 10 elements (1 min of data)
					this.history[name] = this.history[name].slice(0, 10);
				}

				let previous = null;
				for (const entry of Object.values(this.history[name])) {
					if (previous !== null) {
						if (JSON.stringify(previous.value) != JSON.stringify(entry.value)) {
							return entry;
						}
					}
					previous = entry;
				}

				return this.history[name][this.history[name].length - 1];
			},
			getRate(name, map) {
				const previous = this.addHistory(name, map);

				let diff = {};
				const delayS = (Date.now() - previous.timestamp) / 1000;
				for (const id in map) {
					diff[id] = {};
					for (const key in map[id]) {
						diff[id][key] = (map[id][key] - ((previous.value[id] || {})[key] || 0)) / delayS;
					}
				}

				return diff;
			},
			makeRateTooltip(displayName, map, mapping) {
				return this.makeTooltip(displayName, map, (item) => {
					const output = Object.keys(mapping)
						.filter((key) => key in item)
						.map((key) => {
							return mapping[key] + ": " + this.formatBytesRate(item[key]);
						});
					return output.join(", ");
				});
			},
			formatBytesRate(rate) {
				return bytesToString(rate) + "/s";
			},
			makeTooltipSingle(displayName, name, data) {
				return this.makeTooltipMulti(displayName, { [name]: data });
			},
			makeTooltipMulti(displayName, map) {
				const messageList = Object.entries(map).map(([name, data]) => {
					return "<li>" + capitalize(name) + ": " + data.display + "</li>";
				});
				return { data: displayName + "<ul>" + messageList.join("\n") + "</ul>" };
			},
			makeTooltip(displayName, map, callback) {
				const messageList = Object.keys(map).map((key) => {
					return "<li>" + (key != "undefined" ? capitalize(key) + ": " : "") + callback(map[key]) + "</li>";
				});
				return { data: displayName + "<ul>" + messageList.join("\n") + "</ul>" };
			},
			/**
			 * From a positive value, return a rate from 0 to 1.
			 */
			makeRate(x) {
				return -1 / Math.log(x + Math.exp(1)) + 1;
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/icons.scss" as * with (
		$bzdIconNames: thermometer battery
	);

	.system-monitor {
		.header {
			text-align: right;
			margin-bottom: 20px;
			.entry {
				margin-left: 20px;
			}
		}

		.gauges {
			width: 100%;
			padding-right: 10px;

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
		}
	}
</style>
