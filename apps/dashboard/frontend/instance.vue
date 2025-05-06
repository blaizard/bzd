<template>
	<div v-for="(tile, key) in tiles" :style="getStyle(key)" :class="getClass(key)" :key="uid + '-' + key">
		<div :class="getTileClass(key)" @click="handleClick(key)">
			<component
				class="content"
				:is="component"
				:description="description"
				:metadata="tile.data"
				:color="getColorForeground(key)"
				:background-color="getColorBackground(key)"
				@color="handleColor(key, $event)"
				@link="handleLink(key, $event)"
				@error="handleError(key, $event)"
				@status="handleStatus(key, $event)"
				@active="handleActive(key, $event)"
				@event="handleEvent(key, $event)"
				@name="handleName(key, $event)"
				@image="handleImage(key, $event)"
				v-tooltip="tooltipTile(key)"
			>
			</component>
			<div class="name"><i :class="icon"></i> {{ getName(key) }}</div>
			<div class="message" v-if="tooltipMessage(key)" v-tooltip="tooltipMessage(key)">
				<i :class="tooltipMessageIcon(key)"></i>
			</div>
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
			defaultColorBackground() {
				if (this.visualizationColor == "auto") {
					return this.color || this.colorAuto;
				}
				return this.visualizationColor;
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
			component() {
				if (!(this.visualizationType in Plugins)) {
					Log.error("Unsupported plugin '{}'", this.visualizationType);
					return null;
				}
				return defineAsyncComponent(() => Plugins[this.visualizationType].module());
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
			tooltipTile(key) {
				if (this.description["tooltip"]) {
					return {
						type: "text",
						data: this.description["tooltip"],
					};
				}
				return false;
			},
			tooltipMessageIcon(key) {
				if (this.tiles[key].tooltipIcon) {
					return this.tiles[key].tooltipIcon;
				}
				return "bzd-icon-email";
			},
			tooltipMessage(key) {
				if (this.tiles[key].tooltip) {
					return this.tiles[key].tooltip;
				}
				return false;
			},
			getColorBackground(key) {
				return this.tiles[key].color || this.defaultColorBackground;
			},
			getColorForeground(key) {
				const color = this.getColorBackground(key);
				return this.getColorForegroundFromBackground(color);
			},
			getColorForegroundFromBackground(color) {
				return (
					{
						white: "black",
					}[color] || "white"
				);
			},
			getStyle(key) {
				const colorBackground = this.getColorBackground(key);
				const colorForeground = this.getColorForegroundFromBackground(colorBackground);
				let style = {
					"background-color": Colors[colorBackground],
					color: Colors[colorForeground],
					"border-color": Colors[colorForeground],
					"--bzd-color-foreground": Colors[colorForeground],
					"--bzd-color-background": Colors[colorBackground],
				};

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
			handleError(key, e) {
				this.handleStatus(key, ["error", e]);
			},
			handleStatus(key, value) {
				Exception.assert(Array.isArray(value), "Status must be an array, not: {:j}", value);
				Exception.assert(value.length, "Status must be an array of 2 elements, not: {:j}", value);
				const [status, tooltip] = value;

				const statusToData = {
					success: { color: "green", icon: "bzd-icon-email" },
					warning: { color: "orange", icon: "bzd-icon-warning" },
					progress: { color: "orange", icon: "bzd-icon-email" },
					error: { color: "red", icon: "bzd-icon-error" },
				};
				Exception.assert(status in statusToData, "Unsupported status: {}", status);
				this.handleColor(key, statusToData[status].color);

				if (tooltip) {
					this.tiles[key].tooltipIcon = statusToData[status].icon;
					this.tiles[key].tooltip = {
						type: "html",
						data: this.tooltipFormat(tooltip),
					};
				} else {
					delete this.tiles[key].tooltip;
				}
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
			/// Format a tooltip into a printable tooltip.
			tooltipFormat(tooltip) {
				const tooltipFormatAsObject = (data) => {
					let entries = [];
					for (const [key, value] of Object.entries(data)) {
						entries.push('<li style="white-space: nowrap;">' + key + ": " + this.tooltipFormat(value) + "</li>");
					}
					return "<ul>" + entries.join("\n") + "</ul>";
				};
				if (typeof tooltip === "string") {
					return tooltip;
				}
				return tooltipFormatAsObject(tooltip);
			},
			// Assign the new tiles.
			assignTilesData(data) {
				for (const [key, tile] of Object.entries(data)) {
					const update = Object.assign(
						this.tiles[key] || {
							active: true,
							link: null,
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
					Log.error("Error while fetching data: " + String(e));
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
				}
			},
		},
	};
</script>

<style lang="scss">
	@use "@/nodejs/icons.scss" as icons with (
		$bzdIconNames: email warning error
	);
</style>

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

			.message {
				position: absolute;
				bottom: 0;
				right: 0;
				width: 20px;
				height: 20px;
				background-color: var(--bzd-color-foreground);
				color: var(--bzd-color-background);

				font-size: 20px;
				line-height: 1.5em;
				width: 1.5em;
				height: 1.5em;
				text-align: center;

				@extend %bzd-clickable;
			}
		}
	}
</style>
