<template>
	<div v-for="(tile, key) in tiles" :style="getStyle(key)" :class="getClass(key)" :key="uid + '-' + key">
		<div :class="getTileClass(key)" :style="containerStyle" @click="handleClick(key)">
			<div v-if="getNbErrors(key) > 0" class="error" v-tooltip="tooltipErrorConfig(key)">{{ getNbErrors(key) }}</div>
			<component
				v-else
				class="content"
				:is="component"
				:description="description"
				:metadata="tile.data"
				:color="colorForeground"
				:background-color="colorBackground"
				@color="handleColor(key, $event)"
				@link="handleLink(key, $event)"
				@error="handleError(key, $event)"
				@active="handleActive(key, $event)"
				@event="handleEvent(key, $event)"
				@name="handleName(key, $event)"
				@image="handleImage(key, $event)"
				v-tooltip="tooltip"
			>
			</component>
			<!--<div v-else-if="isError" class="content">Fatal error</div>//-->
			<div class="name"><i :class="icon"></i> {{ getName(key) }}</div>
		</div>
	</div>
</template>

<script>
	import Colors from "#bzd/nodejs/styles/default/css/colors.module.scss";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.mjs";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import Plugins from "../plugins/plugins.frontend.index.mjs";
	import Color from "#bzd/nodejs/utils/color.mjs";
	import LogFactory from "#bzd/nodejs/core/log.mjs";
	import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
	import { defineAsyncComponent } from "vue";

	const Log = LogFactory("instance");
	const Exception = ExceptionFactory("instance");

	export default {
		props: {
			description: { type: Object, mandatory: true },
			uid: { type: String, mandatory: true },
		},
		directives: {
			loading: DirectiveLoading,
			tooltip: DirectiveTooltip,
		},
		data: function () {
			return {
				tiles: {},
				handleTimeout: null,
				color: null,
				icon: null,
			};
		},
		mounted() {
			// If this tile has a backend, fetch the new data.
			if (this.sourceType) {
				this.fetch();
			}
			// Otherwise add a tile.
			else {
				this.assignTilesData({ "": {} });
			}
			this.fetchIcon();
		},
		beforeUnmount() {
			if (this.handleTimeout) {
				clearTimeout(this.handleTimeout);
			}
		},
		computed: {
			colorAuto() {
				const index = Array.from(this.uid).reduce((acc, c) => acc + parseInt(c.charCodeAt(0)), 0);
				const colorList = Object.keys(Colors);
				return colorList[index % colorList.length];
			},
			colorBackground() {
				if (this.visualizationColor == "auto") {
					return this.color || this.colorAuto;
				}
				return this.visualizationColor;
			},
			colorForeground() {
				return (
					{
						white: "black",
					}[this.colorBackground] || "white"
				);
			},
			visualizationColor() {
				return this.description["visualization.color"] || "auto";
			},
			visualizationType() {
				return this.description["visualization.type"] || null;
			},
			sourceType() {
				return this.description["source.type"] || null;
			},
			timeout() {
				return Plugins[this.sourceType].metadata.timeout || 60;
			},
			tooltip() {
				if (this.description["tooltip"]) {
					return {
						type: "text",
						data: this.description["tooltip"],
					};
				}
				return false;
			},
			component() {
				if (!(this.visualizationType in Plugins)) {
					Log.error("Unsupported plugin '{}'", this.visualizationType);
					return null;
				}
				return defineAsyncComponent(() => Plugins[this.visualizationType].module());
			},
			containerStyle() {
				let color = new Color(Colors[this.colorBackground]);
				color.setAlpha(0.6);
				const backgroundColor = Colors[this.colorBackground];
				return (
					"background-color: " +
					backgroundColor +
					"; color: " +
					Colors[this.colorForeground] +
					"; border-color: " +
					Colors[this.colorForeground] +
					"; --bzd-loading-color:" +
					Colors[this.colorForeground] +
					";"
				);
			},
		},
		methods: {
			getName(key) {
				if (this.tiles[key].name) {
					return this.tiles[key].name;
				}
				let names = [];
				if (this.description.name) {
					names.push(this.description.name);
				}
				if (key) {
					names.push(key);
				}
				return names.join(".");
			},
			getTileClass(key) {
				return {
					container: true,
					clickable: Boolean(this.tiles[key].link),
				};
			},
			getStyle(key) {
				let style = {};
				if (this.tiles[key].image) {
					style["background-image"] = "url('" + this.tiles[key].image + "')";
				}
				return style;
			},
			getClass(key) {
				return {
					"bzd-dashboard-tile": true,
					active: this.tiles[key].active,
				};
			},
			tooltipErrorConfig(key) {
				return {
					type: "text",
					data: this.tiles[key].errors.map((e) => String(e)).join(", "),
				};
			},
			getNbErrors(key) {
				return (this.tiles[key].errors || []).length;
			},
			handleInstanceError(e) {
				Log.error(e);
			},
			handleError(key, e) {
				this.tiles[key].errors ??= [];
				this.tiles[key].errors.push(e);
			},
			handleActive(key, active) {
				this.tiles[key].active = Boolean(active);
			},
			/// This is called once a new tile is created.
			handleNewTile(key) {
				if (this.description.link) {
					this.handleLink(key, this.description.link);
				}
			},
			// Assign the new tiles.
			assignTilesData(data) {
				for (const [key, tile] of Object.entries(data)) {
					const update = Object.assign(
						this.tiles[key] || {
							active: true,
							link: null,
							errors: [],
						},
						{
							data: tile,
							timestamp: Date.now(),
						},
					);
					const newTile = !(key in this.tiles);
					this.tiles[key] = update;
					if (newTile) {
						this.handleNewTile(key);
					}
				}
			},
			async fetch() {
				this.handleTimeout = null;
				try {
					const data = await this.$rest.request("get", "/data", {
						uid: this.uid,
					});

					if (Array.isArray(data)) {
						const tiles = data.reduce((tiles, tile, index) => {
							const key = tile.key || index;
							Exception.assert(!(key in tiles), "At least 2 tiles have the same key.");
							tiles[key] = tile;
							return tiles;
						}, {});
						this.assignTilesData(tiles);
					} else {
						const tiles = {
							"": data,
						};
						this.assignTilesData(tiles);
					}
					this.handleTimeout = setTimeout(this.fetch, this.timeout);
				} catch (e) {
					this.handleInstanceError("Error while fetching data: " + String(e));
				}
			},
			async fetchIcon() {
				const plugin = this.sourceType ? Plugins[this.sourceType] : Plugins[this.visualizationType];
				if (plugin && "module" in plugin) {
					await plugin.module(); // Load the frontend plugin to load the icon
					this.icon = plugin.metadata.icon || "";
				}
			},
			handleClick(key) {
				if (this.tiles[key].link) {
					window.open(this.tiles[key].link);
				}
			},
			handleColor(key, color) {
				this.tiles[key].color = color;
			},
			handleLink(key, link) {
				this.tiles[key].link = link;
				// Check if the link is valid.
				// Due to CORS this method doesn't work, I should use a proxy via the server.
				this.$rest
					.request("get", "/check-url", {
						url: link,
					})
					.then((data) => {
						this.handleActive(key, data.valid);
					});
			},
			handleName(key, name) {
				this.tiles[key].name = name;
			},
			handleImage(key, image) {
				this.tiles[key].image = image;
			},
			async handleEvent(key, type) {
				try {
					await this.$rest.request("post", "/event", {
						uid: this.uid,
						key: key,
						event: type,
					});
				} catch (e) {
					this.handleError(key, "Error while triggering event: " + String(e));
					this.handleColor(key, "red");
				}
			},
		},
	};
</script>

<style lang="scss" scoped>
	@use "@/nodejs/styles/default/css/clickable.scss" as *;
	@use "@/config.scss" as config;

	$bzdPadding: 10px;

	.bzd-dashboard-tile {
		width: 300px;
		height: 300px;
		margin: 1px;
		padding: 0px;

		background-repeat: no-repeat;
		background-size: contain;
		background-position: center;

		&:not(.active) {
			opacity: 0.3;
		}

		.container {
			width: 300px;
			height: 300px;

			border-width: 1px;
			border-style: solid;
			position: relative;

			&.clickable {
				@extend %bzd-clickable;
			}

			.name {
				position: absolute;
				bottom: $bzdPadding;
				left: $bzdPadding;
				right: $bzdPadding;
				overflow: hidden;
				white-space: nowrap;
				text-overflow: ellipsis;
			}

			.content {
				position: absolute;
				top: $bzdPadding;
				bottom: calc(#{$bzdPadding * 2} + 2em);
				left: $bzdPadding;
				right: $bzdPadding;
				overflow: hidden;
			}

			.error {
				position: absolute;
				right: -1em;
				top: -1em;
				width: 2em;
				height: 2em;
				border-radius: 1em;
				border: 1px solid config.$bzdGraphColorWhite;
				z-index: 1;
				color: config.$bzdGraphColorWhite;
				background-color: config.$bzdGraphColorRed;
				line-height: 2em;
				text-align: center;
			}
		}
	}
</style>
