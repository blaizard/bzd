<template>
	<div :class="elementClass">
		<div :class="name + '-content irgraph-content'">
			<slot v-bind:selected="selectedLegend"></slot>
		</div>
		<div :class="legendClass" ref="legend">
			<div class="irgraph-legend-items" v-hover-children="selectedLegend">
				<div v-for="item, index in value"
						:key="index"
						:class="getLegendItemClass(index)">
					<span class="irgraph-legend-item-color">&nbsp;</span> {{ item.caption }}
				</div>
			</div>
		</div>
	</div>
</template>

<script>
	"use strict";

	import HoverChildren from "./directive/hover-children.js"

	export default {
		props: {
			name: {type: String, required: true},
			value: {type: Array, required: false, default: () => []},
			config: {type: Object, required: false, default: () => ({})},
			selected: {type: Number, required: false, default: -1}
		},
		directives: {
			"hover-children": HoverChildren
		},
		data() {
			return {
				selectedLegend: -1
			};
		},
		computed: {
			configProcessed() {
				return Object.assign({
					layout: "legend-right" // legend-left
				}, this.config);
			},
			elementClass() {
				return {
					"irgraph": true,
					[this.name]: true,
					["irgraph-layout-" + this.configProcessed.layout]: true
				};
			},
			legendClass() {
				return {
					"irgraph-legend": true,
					[this.name + "-legend"]: true
				};
			},
			indexSelected() {
				return (this.selected === -1) ? this.selectedLegend : this.selected;
			}
		},
		methods: {
			getLegendItemClass(index) {
				return {
					"irgraph-legend-item": true,
					"irgraph-selected": (this.indexSelected === index)
				}
			}
		}
	}
</script>
