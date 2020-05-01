<template>
	<div :class="tileClass" @click="handleClick">
        <component class="content" :is="component" :metadata="metadata"></component>
        <div class="name">{{ name }}</div>
	</div>
</template>

<script>
	"use strict"

    import { Visualization, Source } from "[dashboard]/plugins/plugins.js";

	export default {
        props: {
            description: {type: Object, mandatory: true},
            uid: {type: String, mandatory: true}
        },
		data: function () {
			return {
                metadata: {},
                handleTimeout: null
			}
		},
        mounted() {
            this.fetch();
        },
        beforeDestroy() {
            if (this.handleTimeout) {
                clearTimeout(this.handleTimeout);
            }
        },
        computed: {
            name() {
                return this.description.name || "<no name>";
            },
            visualizationType() {
                return (this.description["visualization.type"] || "").toLowerCase();
            },
            sourceType() {
                return (this.description["source.type"] || "").toLowerCase();
            },
            timeout() {
                return Source[this.sourceType].timeout || 0;
            },
            component() {
                return (Visualization[this.visualizationType] || {}).frontend;
            },
            tileClass() {
                return {
                    "bzd-dashboard-tile": true,
                    ["bzd-dashboard-color-" + this.description["visualization.color"]]: true
                }
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
			handleClick() {
				this.$routerDispatch("/update/" + this.uid);
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
