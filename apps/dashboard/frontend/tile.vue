<template>
	<div class="bzd-dashboard-tile">
        <component class="content" :is="component" :metadata="metadata"></component>
        <div class="name">{{ name }}</div>
	</div>
</template>

<script>
	"use strict"

    import { Frontend } from "[dashboard]/plugins/plugins.js";

	export default {
        props: {
            description: {type: Object, mandatory: true},
            uid: {type: String, mandatory: true}
        },
		data: function () {
			return {
                metadata: {}
			}
		},
        mounted() {
            this.fetch();
        },
        computed: {
            name() {
                return this.description.name || "<no name>";
            },
            type() {
                return (this.description.type || "").toLowerCase();
            },
            component() {
                return Frontend[this.type];
            }
        },
        methods: {
            async fetch() {
                this.metadata = await this.$api.request("get", "/data", {
                    type: this.type,
                    uid: this.uid
                });
                console.log(this.metadata);
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
