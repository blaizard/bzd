<template>
	<div class="bzd-dashboard-tile" :style="tileStyle" @click="handleClick">
        <component class="content"
                :is="component"
                :metadata="metadata"
                :color="colorForeground"
                :background-color="colorBackground"
                @color="handleColor">
        </component>
        <div class="name"><i :class="icon"></i> {{ name }}</div>
	</div>
</template>

<script>
	"use strict"

    import { Visualization, Source } from "[dashboard]/plugins/plugins.js";
    import Colors from "[bzd-style]/css/colors.scss";

	export default {
        props: {
            description: {type: Object, mandatory: true},
            uid: {type: String, mandatory: true}
        },
		data: function () {
			return {
                metadata: {},
                handleTimeout: null,
                color: null,
                icon: null
			}
		},
        mounted() {
            this.fetch();
            this.fetchIcon();
        },
        beforeDestroy() {
            if (this.handleTimeout) {
                clearTimeout(this.handleTimeout);
            }
        },
        computed: {
            colorAuto() {
                const index = Array.from(this.uid).reduce((acc, c) => acc + c.charCodeAt(0));
                const colorList = Object.keys(Colors);
                return colorList[index % colorList.length];
            },
            colorBackground() {
                if (this.description["visualization.color"] == "auto") {
                    return this.color || this.colorAuto;
                }
                return this.description["visualization.color"];
            },
            colorForeground() {
                return {
                    yellow: "black",
                    white: "black",
                    pink: "black"
                }[this.colorBackground] || "white";
            },
            name() {
                return this.description.name || "<no name>";
            },
            visualizationType() {
                return (this.description["visualization.type"] || "");
            },
            sourceType() {
                return (this.description["source.type"] || "");
            },
            timeout() {
                return Source[this.sourceType].timeout || 0;
            },
            component() {
                return (Visualization[this.visualizationType] || {}).frontend;
            },
            tileStyle() {
                return "background-color: " + Colors[this.colorBackground] + "; color: " + Colors[this.colorForeground] + ";";
            }
        },
        methods: {
            async fetch() {
                this.handleTimeout = null;
                this.metadata = await this.$api.request("get", "/data", {
                    uid: this.uid,
                    type: this.sourceType
                });
                this.handleTimeout = setTimeout(this.fetch, this.timeout);
            },
            async fetchIcon() {
                const frontend = (await Source[this.sourceType].frontend()).default;
                this.icon = frontend.methods.getMetadata().icon;
            },
			handleClick() {
				this.$routerDispatch("/update/" + this.uid);
			},
            handleColor(color) {
                this.color = color;
            }
        }
	}
</script>

<style lang="scss" scoped>
    $bzdPadding: 10px;

    .bzd-dashboard-tile {
        width: 300px;
        height: 300px;
        margin: 10px;

        border: 1px solid #ddd;
        position: relative;

        .name {
            position: absolute;
            bottom: $bzdPadding;
            left: $bzdPadding;
            right: $bzdPadding;
            overflow: hidden;
            line-height: 2em;
        }

        .content {
            position: absolute;
            top: $bzdPadding;
            bottom: calc(#{$bzdPadding * 2} + 2em);
            left: $bzdPadding;
            right: $bzdPadding;
            overflow: hidden;
        }
    }
</style>
