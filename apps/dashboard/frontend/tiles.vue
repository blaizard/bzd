<template>
	<div class="bzd-dashboard-tiles">
        <Tile v-for="description, uid in tiles"
				:key="uid"
				:uid="uid"
				:class="tileClass"
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
		props: {
			edit: {type: Boolean, mandatory: false, default: false},
			another: {type: String, mandatory: false, default: null},
		},
		data: function () {
			return {
				tiles: {}
			}
		},
		mounted() {
			this.getTiles();
		},
		computed: {
			tileClass() {
				return {
					"bzd-dashboard-tile": true,
					"edit": this.edit
				}
			}
		},
		methods: {
			async getTiles() {
				this.tiles = await this.$api.request("get", "/tiles");
				console.log(this.tiles);
			}
		}
	}
</script>

<style lang="scss">
    .bzd-dashboard-tiles {
		display: flex;
		flex-flow: row wrap;

		.bzd-dashboard-tile.edit:nth-child(2n) {
			animation-name: bzd-jiggle-1;
			animation-iteration-count: infinite;
			animation-direction: alternate;
			animation-duration: .2s;
			transform-origin: 50% 10%;
		}

		.bzd-dashboard-tile.edit:nth-child(2n-1) {
			animation-name: bzd-jiggle-2;
			animation-iteration-count: infinite;
			animation-direction: alternate;
			animation-duration: .2s;
			transform-origin: 30% 5%;
		}

		@keyframes bzd-jiggle-1 {
			0% {
				transform: rotate(-1deg);
				animation-timing-function: ease-in;
			}

			50% {
				transform: rotate(1.5deg);
				animation-timing-function: ease-out;
			}
		}

		@keyframes bzd-jiggle-2 {
			0% {
				transform: rotate(1deg);
				animation-timing-function: ease-in;
			}

			50% {
				transform: rotate(-1.5deg);
				animation-timing-function: ease-out;
			}
		}
	}
</style>
