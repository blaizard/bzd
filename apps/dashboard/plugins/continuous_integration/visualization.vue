<template>
	<div>
		<div class="status-header">
			<div v-if="isValid">
				<i :class="statusMap[lastBuildStatus].icon"></i>
				{{ lastBuildStatusDisplay }}
			</div>
			<div v-if="isValid">
				<i class="bzd-icon-clock"></i>
				<span
					>{{ lastBuildDate[0] }}<small>{{ lastBuildDate[1] }} ago</small></span
				>
			</div>
		</div>
		<div class="metrics">
			<div v-if="isValid" v-tooltip="tooltipBuilds">
				<div class="name">Builds</div>
				<div class="value">{{ buildPerWeek }}<small>/week</small></div>
			</div>
			<div v-if="isValid" v-tooltip="tooltipReliability">
				<div class="name">Reliability</div>
				<div class="value">{{ buildReliability }}<small>%</small></div>
			</div>
			<div v-if="isValid" v-tooltip="tooltipSpeed">
				<div class="name">Speed</div>
				<div class="value">
					{{ avgBuildDuration[0] }}<small>{{ avgBuildDuration[1] }}</small>
				</div>
			</div>
		</div>
		<Plot v-if="isValid" class="builds" :config="plotConfig" :value="plotValue"></Plot>
	</div>
</template>

<script>
	import Plot from "#bzd/nodejs/vue/components/graph/plot.vue";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";

	export default {
		components: {
			Plot,
		},
		props: {
			metadata: { type: Object, mandatory: true },
			color: { type: String, mandatory: true },
		},
		directives: {
			tooltip: DirectiveTooltip,
		},
		mounted() {
			this.instanceInterval = setInterval(() => {
				this.timestamp = Date.now();
			}, 1000);
		},
		beforeUnmount() {
			clearInterval(this.instanceInterval);
		},
		data: function () {
			return {
				timestamp: Date.now(),
				instanceInterval: null,
			};
		},
		watch: {
			lastBuild: {
				immediate: true,
				handler() {
					if (this.isValid) {
						this.$emit("color", this.statusMap[this.lastBuildStatus].color);
						if (this.lastBuild.link) {
							this.$emit("link", this.lastBuild.link);
						}
					}
				},
			},
		},
		computed: {
			tooltipSpeed() {
				return {
					data: "Average successful build speed over the last 14 days.<br/><small>(or the last successful build if none is available)</small>",
				};
			},
			tooltipReliability() {
				return { data: "Success rate over the last 30 days." };
			},
			tooltipBuilds() {
				return { data: "Average number of build per 7 days.<br/><small>(taken over a period of 14 days)</small>" };
			},
			statusMap() {
				return {
					success: {
						color: "green",
						display: "Success",
						icon: "bzd-icon-status_success",
					},
					failure: {
						color: "red",
						display: "Failure",
						icon: "bzd-icon-status_failure",
					},
					"in-progress": {
						color: "orange",
						display: "In Progress",
						icon: "bzd-icon-status_in_progress bzd-icon-spin",
					},
					abort: {
						color: "gray",
						display: "Aborted",
						icon: "bzd-icon-status_abort",
					},
					unknown: {
						color: "gray",
						display: "Unknown",
						icon: "",
					},
				};
			},
			plotConfig() {
				return {
					minY: 0,
					minX: 0,
					maxX: 30,
					showLegend: false,
					showAxisX: false,
					showAxisY: false,
					showCursor: false,
					paddingLeft: 5,
					paddingRight: 5,
					formatY: (y) => (Number(y) / 1000 / 60).toFixed(1) + " min",
				};
			},
			builds() {
				let copy = [...(this.metadata.builds || [])];
				copy.sort((a, b) => b.timestamp - a.timestamp);
				return copy.filter((build) => build.status != "abort");
			},
			plotValue() {
				let last30Builds = this.builds.slice(0, 30).reverse();
				return [
					{
						caption: "builds",
						type: "bar",
						color: this.color,
						values: last30Builds.map((build, index) => [index, build.duration]),
						tooltip: (x, y) => {
							const duration = this.getDuration(y);
							const build = last30Builds[x];
							const date = this.getDuration(this.timestamp - build.timestamp);
							return (
								this.statusMap[build.status].display +
								"<br/>" +
								date[0] +
								"<small>" +
								date[1] +
								" ago</small><br/>" +
								"Speed: " +
								duration[0] +
								"<small>" +
								duration[1] +
								"</small>"
							);
						},
					},
				];
			},
			lastBuildDate() {
				return this.getDuration(this.timestamp - this.builds[0].timestamp);
			},
			avgBuildDuration() {
				let buildList = this.getLastBuilds(14).filter((build) => build.status == "success");
				// If no elements
				if (!buildList) {
					buildList = this.builds.filter((build) => build.status == "success");
					if (buildList) {
						buildList = [buildList[0]];
					}
				}

				// Build the average
				const sum = buildList.reduce((sum, build) => sum + build.duration, 0);
				return sum ? this.getDuration(sum / buildList.length) : this.getDuration();
			},
			lastBuild() {
				return this.isValid ? this.builds[0] : null;
			},
			lastBuildStatus() {
				if (this.lastBuild && this.lastBuild.status in this.statusMap) {
					return this.lastBuild.status;
				}
				return "unknown";
			},
			lastBuildStatusDisplay() {
				return this.statusMap[this.lastBuildStatus].display;
			},
			isValid() {
				return this.builds.length > 0;
			},
			buildPerWeek() {
				const buildsLast2Weeks = this.getLastBuilds(14);
				const nbBuilds = buildsLast2Weeks.length;
				if (nbBuilds > 0) {
					return Math.round(buildsLast2Weeks.length / 2);
				}
				return 0;
			},
			/**
			 * Look at the success rate over the last 30 days.
			 * Integer expressed in percent.
			 */
			buildReliability() {
				const buildsLast30Days = this.getLastBuilds(30);
				const nbSuccessfullBuilds = buildsLast30Days.reduce((total, build) => {
					return total + (build.status == "success" ? 1 : 0);
				}, 0);
				const nbFailureSuccessdBuilds = buildsLast30Days.reduce((total, build) => {
					return total + (["success", "failure"].includes(build.status) ? 1 : 0);
				}, 0);
				return nbFailureSuccessdBuilds ? Math.round((nbSuccessfullBuilds / nbFailureSuccessdBuilds) * 100) : 0;
			},
		},
		methods: {
			getDuration(timestampDuration) {
				if (timestampDuration == undefined) {
					return ["-", ""];
				}
				timestampDuration /= 1000;
				if (timestampDuration < 60) {
					return [timestampDuration.toFixed(0), "s"];
				}
				timestampDuration /= 60;
				if (timestampDuration < 60) {
					return [timestampDuration.toFixed(1), "m"];
				}
				timestampDuration /= 60;
				if (timestampDuration < 24) {
					return [timestampDuration.toFixed(1), "h"];
				}
				timestampDuration /= 24;
				return [timestampDuration.toFixed(1), "d"];
			},
			getLastBuilds(days) {
				const timestamp = this.timestamp - days * 24 * 60 * 60 * 1000;
				return this.builds.filter((build) => build.timestamp > timestamp);
			},
		},
	};
</script>

<style lang="scss">
	@use "#bzd/nodejs/icons.scss" as icons with (
		$bzdIconNames: status_success status_failure status_in_progress status_abort clock
	);

	.bzd-icon-continuous_integration {
		@include icons.defineIcon("continuous_integration.svg");
	}
</style>

<style lang="scss" scoped>
	.status-header {
		height: 20%;

		display: flex;
		flex-flow: row nowrap;
		justify-content: space-between;
	}

	.metrics {
		height: 50%;

		display: flex;
		flex-flow: row nowrap;
		justify-content: space-around;
		align-items: center;
		text-align: center;

		.name {
			font-weight: bold;
		}
	}

	.builds {
		height: 30%;
	}
</style>
