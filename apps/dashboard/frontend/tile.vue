<template>
	<div class="bzd-dashboard-tile" :style="tileStyle">
		<div :class="tileClass" :style="containerStyle" @click="handleClick" v-loading="!(initialized || edit)">
			<div v-if="isError" class="error" v-tooltip="tooltipErrorConfig">{{ errorList.length }}</div>
			<component
				v-if="showComponent"
				class="content"
				:is="component"
				:description="description"
				:metadata="metadata"
				:color="colorForeground"
				:background-color="colorBackground"
				@color="handleColor"
				@link="handleLink"
				@error="handleError"
				@clickable="handleClickable"
				@event="handleEvent"
				@name="handleName"
				@image="handleImage"
			>
			</component>
			<div v-else-if="isError" class="content">Fatal error</div>
			<div class="name"><i :class="icon"></i> {{ name }}</div>
		</div>
	</div>
</template>

<script>
	import Colors from "#bzd/nodejs/styles/default/css/colors.scss";
	import DirectiveLoading from "#bzd/nodejs/vue/directives/loading.mjs";
	import DirectiveTooltip from "#bzd/nodejs/vue/directives/tooltip.mjs";
	import Plugins from "../plugins/plugins.frontend.index.mjs";
	import Color from "#bzd/nodejs/utils/color.mjs";
	import LogFactory from "#bzd/nodejs/core/log.mjs";

	const Log = LogFactory("tile");

	export default {
		props: {
			description: { type: Object, mandatory: true },
			uid: { type: String, mandatory: true },
			edit: { type: Boolean, mandatory: false, default: false },
		},
		directives: {
			loading: DirectiveLoading,
			tooltip: DirectiveTooltip,
		},
		data: function () {
			return {
				initialized: true,
				showComponent: true,
				metadata: {},
				handleTimeout: null,
				errorList: [],
				color: null,
				icon: null,
				link: null,
				clickable: false,
				manualName: null,
				image: null,
			};
		},
		mounted() {
			// Set the default link if any
			if ("link.url" in this.description) {
				this.link = this.description["link.url"];
			}
			if (this.sourceType) {
				this.initialized = false;
				this.showComponent = false;
				this.fetch();
			}
			this.fetchIcon();
		},
		beforeDestroy() {
			if (this.handleTimeout) {
				clearTimeout(this.handleTimeout);
			}
		},
		computed: {
			isError() {
				return this.errorList.length > 0;
			},
			tooltipErrorConfig() {
				return {
					type: "text",
					data: this.errorList.map((e) => String(e)).join(", "),
				};
			},
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
						yellow: "black",
						white: "black",
						pink: "black",
					}[this.colorBackground] || "white"
				);
			},
			name() {
				return this.description.name || this.manualName || this.sourceType || this.visualizationType;
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
				return Plugins[this.sourceType].metadata.timeout || 0;
			},
			component() {
				if (!(this.visualizationType in Plugins)) {
					Log.error("Unsupported plugin '{}'", this.visualizationType);
					return null;
				}
				return Plugins[this.visualizationType].module;
			},
			tileClass() {
				return {
					container: true,
					edit: this.edit,
					clickable: Boolean(this.clickable) || Boolean(this.link) || this.edit,
				};
			},
			tileStyle() {
				return 'background-image: url("' + this.image + '");';
			},
			containerStyle() {
				let color = new Color(Colors[this.colorBackground]);
				color.setAlpha(0.6);
				const backgroundColor = this.image ? color.toString() : Colors[this.colorBackground];
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
			handleError(e) {
				this.errorList.push(e);
			},
			async fetch() {
				this.handleTimeout = null;
				try {
					this.metadata = await this.$api.request("get", "/data", {
						uid: this.uid,
						type: this.sourceType,
					});
					this.showComponent = true;
					this.handleTimeout = setTimeout(this.fetch, this.timeout);
				} catch (e) {
					this.handleError("Error while fetching data: " + String(e));
					this.handleColor("red");
				} finally {
					this.initialized = true;
				}
			},
			async fetchIcon() {
				const plugin = this.sourceType ? Plugins[this.sourceType] : Plugins[this.visualizationType];
				if (plugin && "module" in plugin) {
					await plugin.module(); // Load the frontend plugin to load the icon
					this.icon = plugin.metadata.icon || "";
				}
			},
			handleClick() {
				if (this.edit) {
					this.$routerDispatch("/update/" + this.uid);
				} else if (this.link) {
					window.open(this.link);
				}
			},
			handleColor(color) {
				this.color = color;
			},
			handleLink(link) {
				this.link = link;
			},
			handleClickable(clickable) {
				this.clickable = clickable;
			},
			handleName(name) {
				this.manualName = name;
			},
			handleImage(image) {
				this.image = image;
			},
			async handleEvent(type) {
				try {
					this.metadata = await this.$api.request("post", "/event", {
						uid: this.uid,
						type: this.sourceType,
						event: type,
					});
				} catch (e) {
					this.handleError("Error while triggering event: " + String(e));
					this.handleColor("red");
				}
			},
		},
	};
</script>

<style lang="scss" scoped>
	@use "#bzd/nodejs/styles/default/css/clickable.scss" as *;
	@use "#bzd/nodejs/styles/default/css/colors.scss" as colors;

	$bzdPadding: 10px;

	.bzd-dashboard-tile {
		width: 300px;
		height: 300px;
		margin: 1px;
		padding: 0px;

		background-repeat: no-repeat;
		background-size: contain;
		background-position: center;

		.container {
			&.clickable {
				@extend %bzd-clickable;
			}

			width: 300px;
			height: 300px;

			border-width: 1px;
			border-style: solid;
			position: relative;

			&.edit {
				border-style: dashed;
				border-width: 2px;
			}

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

			.error {
				position: absolute;
				right: -1em;
				top: -1em;
				width: 2em;
				height: 2em;
				border-radius: 1em;
				border: 1px solid colors.$bzdGraphColorWhite;
				z-index: 1;
				color: colors.$bzdGraphColorWhite;
				background-color: colors.$bzdGraphColorRed;
				line-height: 2em;
				text-align: center;
			}
		}
	}
</style>
