<template>
	<div class="bzd-dashboard-tiles">
		<Tile v-for="(description, uid) in tiles" :key="uid" :uid="uid" :edit="edit" :description="description"> </Tile>
	</div>
</template>

<script>
	import Tile from "./tile.vue";

	export default {
		components: {
			Tile
		},
		props: {
			edit: { type: Boolean, mandatory: false, default: false },
			another: { type: String, mandatory: false, default: null }
		},
		data: function() {
			return {
				tiles: {}
			};
		},
		mounted() {
			this.getTiles();
		},
		methods: {
			async getTiles() {
				this.tiles = await this.$api.request("get", "/tiles");
			}
		}
	};
</script>

<style lang="scss">
	.bzd-dashboard-tiles {
		display: flex;
		flex-flow: row wrap;
	}
</style>
