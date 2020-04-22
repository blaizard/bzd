<template>
	<div class="bzd-dashboard-tile">
        <div>{{ name }}</div>
        <div>{{ type }}</div>
        <component :is="component"></component>
	</div>
</template>

<script>
	"use strict"

    import Plugins from "[frontend]/plugins.js";

	export default {
        props: {
            description: {type: Object, mandatory: true},
        },
		data: function () {
			return {
			}
		},
        computed: {
            name() {
                return this.description.name || "<no name>";
            },
            type() {
                return (this.description.type || "").toLowerCase();
            },
            component() {
                return Plugins.all[this.type];
            }
        }
	}
</script>

<style lang="scss">
    .bzd-dashboard-tile {
        width: 200px;
        height: 150px;
        margin: 10px;
        padding: 10px;

        border: 1px solid #aaa;
    }
</style>
