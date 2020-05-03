<template>
	<div>
        <div class="status-header">
            <div v-if="isValid">
                <span v-if="lastBuildStatus == 'success'" class="bzd-icon-status-success"></span>
                <span v-else-if="lastBuildStatus == 'failure'" class="bzd-icon-status-failure"></span>
                <span v-else-if="lastBuildStatus == 'in-progress'" class="bzd-icon-status-in-progress bzd-icon-spin"></span>
                <span v-else-if="lastBuildStatus == 'abort'" class="bzd-icon-status-abort"></span>
                {{ lastBuildStatus }}
            </div>
            <div v-if="isValid">
                <i class="bzd-icon-clock"></i>
                <span>{{ lastBuildDate[0] }}<small>{{ lastBuildDate[1] }} ago</small></span>
            </div>
        </div>
        <div v-if="isValid">Buids: {{ buildPerWeek }}<small>/week</small></div>
        <div v-if="isValid">Reliability: {{ buildReliability }}<small>%</small></div>
        <div v-if="isValid">Speed: {{ lastBuildDuration[0] }}<small>{{ lastBuildDuration[1] }}</small></div>
        <Plot class="builds" :config="plotConfig" :value="plotValue"></Plot>
	</div>
</template>

<script>
    "use strict";

    import Plot from "[bzd]/vue/components/graph/plot.vue"; 

    export default {
        components: {
            Plot
        },
        props: {
            metadata: {type: Object, mandatory: true}
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
			}
		},
        computed: {
            plotConfig() {
                return {
                    showLegend: false,
                    showAxisX: false,
                    showAxisY: false,
                    showCursor: false,
                    paddingLeft: 5,
                    paddingRight: 5,
				    formatY: (y) => (Number(y) / 1000 / 60).toFixed(1) + " min",
                }
            },
            builds() {
                return this.metadata.builds || [];
            },
            plotValue() {
                let last30Builds = this.builds.slice(0, 30);
                return [{
                    caption: "builds",
                    type: "bar",
                    color: "white",
                    values: last30Builds.reverse().map((build, index) => [index, build.duration])
                }];
            },
            lastBuildDate() {
                return this.getDuration(this.timestamp - this.builds[0].timestamp);
            },
            lastBuildDuration() {
                return this.getDuration(this.builds[0].duration);
            },
            lastBuildStatus() {
                return this.builds[0].status;
            },
            isValid() {
                return (this.builds.length > 0);
            },
            buildPerWeek() {
                const buildsLastWeek = this.getLastBuilds(7);
                return buildsLastWeek.length;
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
            getMetadata() {
                return {
                    name: "Continuous Integration",
                    icon: "bzd-icon-continuous-integration",
                    form: [
                    ]
                }
            }
        }
    }
</script>

<style lang="scss">
	$bzdIconNames: status-success, status-failure, status-in-progress, status-abort, clock;
	@import "~[bzd]/icons.scss";

    .bzd-icon-continuous-integration {
		@include defineIcon("continuous-integration.svg");
	}
</style>

<style lang="scss" scoped>
    .status-header {
        display: flex;
        flex-flow: row nowrap;
        justify-content: space-between;
    }

    .builds {
        height: 30%;
    }
</style>
