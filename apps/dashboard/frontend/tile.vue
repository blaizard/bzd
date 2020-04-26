<template>
	<div class="bzd-dashboard-tile">
        <component :is="component" :metadata="metadata"></component>
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
