<template>
	<div>
        <div class="status-header">
            <div v-if="isValid">
                <i :class="statusMap[lastBuildStatus].icon"></i>
                {{ lastBuildStatusDisplay }}
            </div>
            <div v-if="isValid">
                <i class="bzd-icon-clock"></i>
                <span>{{ lastBuildDate[0] }}<small>{{ lastBuildDate[1] }} ago</small></span>
            </div>
        </div>
        <div class="metrics">
            <div v-if="isValid">
                <div class="name">Buids</div>
                <div class="value">{{ buildPerWeek }}<small>/week</small></div>
            </div>
            <div v-if="isValid">
                <div class="name">Reliability</div>
                <div class="value">{{ buildReliability }}<small>%</small></div>
            </div>
            <div v-if="isValid">
                <div class="name">Speed</div>
                <div class="value">{{ lastBuildDuration[0] }}<small>{{ lastBuildDuration[1] }}</small></div>
            </div>
        </div>
        <Plot v-if="isValid" class="builds" :config="plotConfig" :value="plotValue"></Plot>
	</div>
</template>

<script>


import Plot from "bzd/vue/components/graph/plot.vue"; 

export default {
	components: {
		Plot
	},
	props: {
		metadata: {type: Object, mandatory: true},
		color: {type: String, mandatory: true},
	},
	mounted() {
		this.instanceInterval = setInterval(() => { this.timestamp = Date.now(); }, 1000);
	},
	beforeDestroy() {
		clearInterval(this.instanceInterval);
	},
	data: function() {
		return {
			timestamp: Date.now(),
			instanceInterval: null
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
			}
		},
	},
	computed: {
		statusMap() {
			return {
				"success": {
					color: "green",
					display: "Success",
					icon: "bzd-icon-status-success"
				},
				"failure": {
					color: "red",
					display: "Failure",
					icon: "bzd-icon-status-failure"
				},
				"in-progress": {
					color: "orange",
					display: "In Progress",
					icon: "bzd-icon-status-in-progress bzd-icon-spin"
				},
				"abort": {
					color: "gray",
					display: "Aborted",
					icon: "bzd-icon-status-abort"
				}
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
			copy.sort((a, b) => (b.timestamp  - a.timestamp));
			return copy;
		},
		plotValue() {
			let last30Builds = this.builds.slice(0, 30).reverse();
			return [{
				caption: "builds",
				type: "bar",
				color: this.color,
				values: last30Builds.map((build, index) => [index, build.duration]),
				tooltip: (x, y) => {
					const duration = this.getDuration(y);
					const build = last30Builds[x];
					const date = this.getDuration(this.timestamp - build.timestamp);
					return this.statusMap[build.status].display + "<br/>"
                                + date[0] + "<small>" + date[1] + " ago</small><br/>"
                                + "Speed: " + duration[0] + "<small>" + duration[1] + "</small>";
				}
			}];
		},
		lastBuildDate() {
			return this.getDuration(this.timestamp - this.builds[0].timestamp);
		},
		lastBuildDuration() {
			// Look for the last completed build
			let index = 0;
			for (const i in this.builds) {
				if (this.builds[i].status == "success" || this.builds[i].status == "failure") {
					index = i;
					break;
				}
			}
			return this.getDuration(this.builds[index].duration);
		},
		lastBuild() {
			return (this.isValid) ? this.builds[0] : null;
		},
		lastBuildStatus() {
			return (this.isValid) ? this.lastBuild.status : null;
		},
		lastBuildStatusDisplay() {
			return this.statusMap[this.lastBuildStatus].display;
		},
		isValid() {
			return (this.builds.length > 0);
		},
		buildPerWeek() {
			const buildsLast2Weeks = this.getLastBuilds(14);
			const nbBuilds = buildsLast2Weeks.length;
			const duration = buildsLast2Weeks[0].timestamp - buildsLast2Weeks[nbBuilds - 1].timestamp;
			return Math.round(buildsLast2Weeks.length / (duration / (7 * 24 * 60 * 60 * 1000)));
		},
		/**
		 * Look at the success rate over the last 30 days.
		 * Integer expressed in percent.
		 */
		buildReliability() {
			const buildsLast30Days = this.getLastBuilds(30);
			const nbSuccessfullBuilds = buildsLast30Days.reduce((total, build) => {
				return total + ((build.status == "success") ? 1 : 0);
			}, 0);
			return Math.round(nbSuccessfullBuilds / buildsLast30Days.length * 100);
		}
	},
	methods: {
		getDuration(timestampDuration) {
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
			return this.builds.filter((build) => (build.timestamp > timestamp));
		},
	}
};
</script>

<style lang="scss">
	@use "bzd/icons.scss" as icons with (
        $bzdIconNames: status-success status-failure status-in-progress status-abort clock
    );

    .bzd-icon-continuous-integration {
		@include icons.defineIcon("continuous-integration.svg");
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
