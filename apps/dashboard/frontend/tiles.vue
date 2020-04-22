<template>
	<div class="bzd-dashboard-tiles">
        <Tile v-for="description, uid in tiles"
				:description="description">
		</Tile>
	</div>
</template>

<script>
	"use strict"

	import Tile from "./tile.vue";

	export default {
		components: {
			Tile
		},
		data: function () {
			return {
				tiles: {}
			}
		},
		mounted() {
			this.getTiles();
		},
		methods: {
			async getTiles() {
				this.tiles = await this.$api.request("get", "/tiles");
			}
		}
	}
</script>

<style lang="scss">
    .bzd-dashboard-tiles {
		display: flex;
		flex-flow: row wrap;
    }
</style>
