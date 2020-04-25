<template>
	<div class="bzd-dashboard-tile">
        <component :is="component"></component>
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
			}
		},
        mounted() {
            this.$api.request("get", "/data", {
                type: this.type,
                uid: this.uid
            });
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
        }
	}
</script>

<style lang="scss" scoped>
    .bzd-dashboard-tile {
        width: 300px;
        height: 150px;
        margin: 10px;
        padding: 10px;

        border: 1px solid #ddd;

        .name {

        }
    }
</style>
