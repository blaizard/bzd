<template>
	<div :class="tileClass" :style="tileStyle" @click="handleClick" v-loading="loading">
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
			@error="handleError">
		</component>
		<div v-else-if="isError" class="content">Fatal error</div>
		<div class="name"><i :class="icon"></i> {{ name }}</div>
	</div>
</template>

<script>
	import Colors from "bzd-style/css/colors.scss";
	import DirectiveLoading from "bzd/vue/directives/loading.mjs";
	import DirectiveTooltip from "bzd/vue/directives/tooltip.mjs";
	import { Visualization, Source } from "../plugins/plugins.mjs";

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
				showComponent: true,
				metadata: {},
				handleTimeout: null,
				loading: false,
				errorList: [],
				color: null,
				icon: null,
				link: null,
			};
		},
		mounted() {
			if (this.sourceType) {
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
					text: this.errorList.map((e) => String(e)).join(", "),
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
				return this.description.name || this.sourceType || this.visualizationType;
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
				return Source[this.sourceType].timeout || 0;
			},
			component() {
				return (Visualization[this.visualizationType] || {}).frontend;
			},
			tileClass() {
				return {
					"bzd-dashboard-tile": true,
					edit: this.edit,
					clickable: Boolean(this.link) || this.edit,
				};
			},
			tileStyle() {
				return (
					"background-color: " +
					Colors[this.colorBackground] +
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
				this.loading = true;
				try {
					this.metadata = await this.$api.request("get", "/data", {
						uid: this.uid,
						type: this.sourceType,
					});
					this.showComponent = true;
					this.handleTimeout = setTimeout(this.fetch, this.timeout);
				}
			catch (e) {
					this.handleError("Error while fetching data: " + String(e));
					this.handleColor("red");
				}
			finally {
					this.loading = false;
				}
			},
			async fetchIcon() {
				const plugin = this.sourceType ? Source[this.sourceType] : Visualization[this.visualizationType];
				if (plugin && "frontend" in plugin) {
					await plugin.frontend(); // Load the frontend plugin to load the icon
					this.icon = plugin.icon;
				}
			},
			handleClick() {
				if (this.edit) {
					this.$routerDispatch("/update/" + this.uid);
				}
			else if (this.link) {
					window.open(this.link);
				}
			},
			handleColor(color) {
				this.color = color;
			},
			handleLink(link) {
				this.link = link;
			},
		},
	};
</script>

<style lang="scss" scoped>
	@use "bzd-style/css/clickable.scss";
	@use "bzd-style/css/colors.scss" as colors;

	$bzdPadding: 10px;

	.bzd-dashboard-tile {
		&.clickable {
			@extend %bzd-clickable;
		}

		width: 300px;
		height: 300px;
		margin: 10px;

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
</style>
