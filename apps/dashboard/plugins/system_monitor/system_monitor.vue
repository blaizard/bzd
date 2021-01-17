<template>
	<div class="system-monitor">
		<!-- Header //-->
		<div class="header">
			<span v-if="isTemperature" class="entry" v-tooltip="temperatureTooltip">
				<i class="bzd-icon-thermometer"></i>
				<span class="value">{{ temperatureMax.toFixed(0) }}°C</span>
			</span>

			<span v-if="isBattery" class="entry">
				<i class="bzd-icon-battery"></i>
				<span class="value">{{ batteryPercent.toFixed(0) }}%</span>
			</span>
		</div>

		<!-- Gauges //-->
		<div class="gauges">
			<div v-if="isCpu" class="gauge" v-tooltip="cpuTooltip">
				<div class="name">CPU</div>
				<div class="value">
					{{ cpuPercent.toFixed(1) }}%
					<div class="bar" :style="cpuStyle"></div>
				</div>
			</div>
			<div v-if="isGpu" class="gauge" v-tooltip="gpuTooltip">
				<div class="name">GPU</div>
				<div class="value">
					{{ gpuPercent.toFixed(1) }}%
					<div class="bar" :style="gpuStyle"></div>
				</div>
			</div>
			<div v-if="isMemory" class="gauge" v-tooltip="memoryTooltip">
				<div class="name">Memory</div>
				<div class="value">
					{{ memoryPercent.toFixed(1) }}%
					<div class="bar" :style="memoryStyle"></div>
				</div>
			</div>
			<div v-if="isSwap" class="gauge" v-tooltip="swapTooltip">
				<div class="name">Swap</div>
				<div class="value">
					{{ swapPercent.toFixed(1) }}%
					<div class="bar" :style="swapStyle"></div>
				</div>
			</div>
		</div>
	</div>
</template>

<script>
	import DirectiveTooltip from "bzd/vue/directives/tooltip.mjs";
	import { bytesToString, capitalize } from "bzd/utils/to_string.mjs";

	export default {
		props: {
			metadata: { type: Object, mandatory: true }
		},
		directives: {
			tooltip: DirectiveTooltip
		},
		data: function() {
			return {};
		},
		computed: {
			isMemory() {
				return this.has("memory.free") && this.has("memory.total");
			},
			memory() {
				return this.makeMemoryMap("memory");
			},
			memoryPercent() {
				return this.makeMemoryPercent(this.memory);
			},
			memoryStyle() {
				return {
					width: this.memoryPercent + "%"
				};
			},
			memoryTooltip() {
				return this.makeMemoryTooltip("Memory", this.memory);
			},
			isSwap() {
				return this.has("swap.free") && this.has("swap.total");
			},
			swap() {
				return this.makeMemoryMap("swap");
			},
			swapPercent() {
				return this.makeMemoryPercent(this.swap);
			},
			swapStyle() {
				return {
					width: this.swapPercent + "%"
				};
			},
			swapTooltip() {
				return this.makeMemoryTooltip("Swap", this.swap);
			},
			isCpu() {
				return this.has("cpu.load");
			},
			cpu() {
				return this.makeCpuMap("cpu");
			},
			cpuPercent() {
				return this.makeCpuPercent(this.cpu);
			},
			cpuStyle() {
				return {
					width: this.cpuPercent + "%"
				};
			},
			cpuTooltip() {
				return this.makeCpuTooltip("CPU", this.cpu);
			},
			isGpu() {
				return this.has("gpu.load");
			},
			gpu() {
				return this.makeCpuMap("gpu");
			},
			gpuPercent() {
				return this.makeCpuPercent(this.gpu);
			},
			gpuStyle() {
				return {
					width: this.gpuPercent + "%"
				};
			},
			gpuTooltip() {
				return this.makeCpuTooltip("GPU", this.gpu);
			},
			isTemperature() {
				return this.has("temperature");
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
			isBattery() {
				return this.has("ups.charge");
			},
			batteryPercent() {
				return this.getItems("ups.charge").reduce((value, item) => Math.min(value, item.value), 1) * 100;
			}
		},
		methods: {
			has(id) {
				for (const i in this.metadata) {
					if (this.metadata[i].id.startsWith(id)) {
						return true;
					}
				}
				return false;
			},
			/**
			 * Get an iterable of all values matching a certain id.
			 */
			getItems(id) {
				return this.metadata.filter((item) => item.id.startsWith(id));
			},
			// CPU
			makeCpuMap(name) {
				return this.getItems(name + ".load").reduce((obj, item) => {
					const id = item.id.substring((name + ".load").length + 1) || "undefined";
					obj[id] = obj[id] || { load: 0 };
					obj[id].load = Math.max(obj[id].load, item.value);
					return obj;
				}, {});
			},
			makeCpuPercent(map) {
				const loadSum = Object.keys(map).reduce((value, key) => value + map[key].load, 0);
				return (loadSum / Object.keys(map).length) * 100;
			},
			makeCpuTooltip(displayName, map) {
				return this.makeTooltip(displayName, map, (item) => {
					return (item.load * 100).toFixed(1) + "% load";
				});
			},
			// Memory
			makeMemoryMap(name) {
				let map = this.getItems(name + ".free").reduce((obj, item) => {
					const id = item.id.substring((name + ".free").length + 1) || "undefined";
					obj[id] = obj[id] || { free: 0, total: 0 };
					obj[id].free += item.value;
					return obj;
				}, {});
				this.getItems(name + ".total").forEach((item) => {
					const id = item.id.substring((name + ".total").length + 1) || "undefined";
					map[id].total += item.value;
				});
				return map;
			},
			makeMemoryPercent(map) {
				const free = Object.keys(map).reduce((value, key) => value + map[key].free, 0);
				const total = Object.keys(map).reduce((value, key) => value + map[key].total, 0);
				return (1 - free / total) * 100;
			},
			makeMemoryTooltip(displayName, map) {
				return this.makeTooltip(displayName, map, (item) => {
					return (
						bytesToString(item.free) +
						" free / " +
						bytesToString(item.total) +
						" (" +
						((item.free / item.total) * 100).toFixed(1) +
						"%)"
					);
				});
			},
			makeTooltip(displayName, map, callback) {
				const messageList = Object.keys(map).map((key) => {
					return "<li>" + (key != "undefined" ? capitalize(key) + ": " : "") + callback(map[key]) + "</li>";
				});
				return { data: displayName + "<ul>" + messageList.join("\n") + "</ul>" };
			}
		}
	};
</script>

<style lang="scss">
	@use "bzd/icons.scss" with (
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

			.gauge {
				width: 100%;
				position: relative;

				.name {
					display: inline-block;
					width: 30%;
					text-align: right;
					padding-right: 10px;
				}
				.value {
					display: inline-block;
					width: 60%;
					overflow: hidden;
					white-space: nowrap;
					position: absolute;
					top: 25%;
					bottom: 25%;
					font-size: 0.5em;
					text-align: right;
					padding-right: 4px;

					&:before {
						content: " ";
						position: absolute;
						left: 0;
						top: 0;
						bottom: 0;
						right: 0;
						background-color: currentColor;
						opacity: 0.2;
					}

					.bar {
						position: absolute;
						top: 0;
						left: 0;
						bottom: 0;
						background-color: currentColor;
						transition: width 0.5s;
					}
				}
			}
		}
	}
</style>
