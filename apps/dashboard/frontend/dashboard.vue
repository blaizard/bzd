<template>
	<div>
		<h1>Dashboard {{ edit }}</h1>
		<RouterComponent ref="view"></RouterComponent>
		{{ tiles }}
	</div>
</template>

<script>
	"use strict"

	export default {
		data: function () {
			return {
				edit: false,
				tiles: {}
			}
		},
		mounted() {
			this.$routerSet({
				ref: "view",
				routes: [
                    { path: '/edit', handler: () => { this.edit = true; } },
                    { path: '/new', component: () => import("[frontend]/config.vue") },
                    { path: '/', component: () => import("[frontend]/tiles.vue") },
				],
			});
			this.getTiles();
		},
		methods: {
			async getTiles() {
				this.tiles = await this.$api.request("get", "/tiles");
			}
		}
	}
</script>
