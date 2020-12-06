<template>
	<div class="system-monitor">
		{{ metadata }}
		<div class="gauges">
			<div v-if="isCpu" class="gauge">
				<div class="name">CPU</div>
				<div class="value">
					{{ cpuPercent.toFixed(1) }}%
					<div class="bar" :style="cpuStyle"></div>
				</div>
			</div>
			<div v-if="isMemory" class="gauge">
				<div class="name">Memory</div>
				<div class="value">
					{{ memoryPercent.toFixed(1) }}%
					<div class="bar" :style="memoryStyle"></div>
				</div>
			</div>
		</div>
	</div>
</template>

<script>
	export default {
		props: {
			metadata: { type: Object, mandatory: true },
		},
		data: function () {
			return {};
		},
		computed: {
			isMemory() {
				return this.has("memory.free") && this.has("memory.total");
			},
			memoryFree() {
				return this.getItems("memory.free").reduce((value, item) => value + item.value, 0);
			},
			memoryTotal() {
				return this.getItems("memory.total").reduce((value, item) => value + item.value, 0);
			},
			memoryPercent() {
				return (1 - this.memoryFree / this.memoryTotal) * 100;
			},
			memoryStyle() {
				return {
					width: this.memoryPercent + "%",
				};
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
					if (this.metadata[i].id == id) {
						return true;
					}
				}
				return false;
			},
			/**
			 * Get an iterable of all values matching a certain id.
			 */
			getItems(id) {
				return this.metadata.filter((item) => item.id == id);
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
						opacity: 0.3;
						transition: width 0.5s;
					}

					.bar {
						position: absolute;
						top: 0;
						left: 0;
						bottom: 0;
						background-color: currentColor;
					}
				}
			}
		}
	}
</style>
