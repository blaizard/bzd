<template>
	<div :class="elementClass" @mouseleave="tooltipHide">
		<div :class="name + '-content irgraph-content'">
			<slot v-bind:selected="selectedLegend"></slot>
		</div>
		<div :class="legendClass" ref="legend" v-if="configProcessed.showLegend">
			<div class="irgraph-legend-items" v-hover-children="selectedLegend">
				<div
					v-for="(item, index) in value"
					:key="index"
					:class="getLegendItemClass(index)"
					:style="getLegendItemStyle(index, item)"
				>
					<span class="irgraph-legend-item-color">&nbsp;</span> {{ item.caption }}
				</div>
			</div>
		</div>
	</div>
</template>

<script>
	import HoverChildren from "./directive/hover_children.mjs";
	import { tooltipHide } from "../../directives/tooltip.mjs";

	export default {
		props: {
			name: { type: String, required: true },
			value: { type: Array, required: false, default: () => [] },
			config: { type: Object, required: false, default: () => ({}) },
			selected: { type: Number, required: false, default: -1 },
		},
		directives: {
			"hover-children": HoverChildren,
		},
		data() {
			return {
				selectedLegend: -1,
			};
		},
		computed: {
			configProcessed() {
				return Object.assign(
					{
						/**
						 * Show the legend
						 */
						showLegend: true,
						/**
						 * Position of the legend
						 */
						legendPosition: "legend-right", // legend-left
					},
					this.config
				);
			},
			elementClass() {
				return {
					irgraph: true,
					[this.name]: true,
					["irgraph-layout-" + this.configProcessed.legendPosition]: true,
				};
			},
			legendClass() {
				return {
					"irgraph-legend": true,
					[this.name + "-legend"]: true,
				};
			},
			indexSelected() {
				return this.selected === -1 ? this.selectedLegend : this.selected;
			},
		},
		methods: {
			getLegendItemClass(index) {
				return {
					"irgraph-legend-item": true,
					"irgraph-selected": this.indexSelected === index,
				};
			},
			getLegendItemStyle(index, item) {
				return "--bzd-graph-color: " + item.color + ";";
			},
			tooltipHide() {
				tooltipHide();
			},
		},
	};
</script>
