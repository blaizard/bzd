<template>
	<div class="system-monitor">
		<div class="gauges">
			<div v-if="isCpu" class="gauge">
				<div class="name">CPU</div>
				<div class="value">
					{{ cpuPercent.toFixed(1) }}%
					<div class="bar" :style="cpuStyle"></div>
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
			metadata: { type: Object, mandatory: true },
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		data: function () {
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
					width: this.memoryPercent + "%",
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
					width: this.swapPercent + "%",
				};
			},
			swapTooltip() {
				return this.makeMemoryTooltip("Swap", this.swap);
			},
			isCpu() {
				return this.has("cpu.load");
			},
			cpuPercent() {
				const cpuLoads = this.getItems("cpu.load");
				const cpuLoadSum = this.getItems("cpu.load").reduce((value, item) => value + item.value, 0);
				return (cpuLoadSum / cpuLoads.length) * 100;
			},
			cpuStyle() {
				return {
					width: this.cpuPercent + "%",
				};
			},
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
				const messageList = Object.keys(map).map((key) => {
					return (
						"<li>" +
						(key != "undefined" ? capitalize(key) + ": " : "") +
						bytesToString(map[key].free) +
						" free / " +
						bytesToString(map[key].total) +
						" (" +
						((map[key].free / map[key].total) * 100).toFixed(1) +
						"%)</li>"
					);
				});
				return { text: displayName + "<ul>" + messageList.join("\n") + "</ul>" };
			},
		},
	};
</script>

<style lang="scss">
	@use "bzd/icons.scss" with (
        $bzdIconNames: play pause stop next previous
    );

	.system-monitor {
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
