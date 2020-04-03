<template>
	<Element name="irgraph-plot" :value="series" :config="config" :selected="selected" v-slot:default="slotProps" v-resize="handleResize">
		<svg ref="canvas" :viewBox="svgViewBox" :style="svgStyle" @mousemove.stop="handleMouseMove">
			<g class="irgraph-plot-labels-x">
				<text class="irgraph-hide" ref="labelXReferenceChar">M</text>
				<template v-for="item, x in labelXMap">
					<text v-if="item.show"
							:x="x"
							:y="height - plotOffsetBottom + tickSize / 2 + labelXCharSize.height"
							text-anchor="middle"
							alignment-baseline="bottom">
						{{ item.value }}
					</text>
					<line v-if="item.show" :x1="x" :x2="x" :y1="height - plotOffsetBottom - tickSize / 2" :y2="height - plotOffsetBottom + tickSize / 2"></line>
				</template>
			</g>
			<g class="irgraph-plot-labels-y">
				<text class="irgraph-hide" ref="labelYReferenceChar">M</text>
				<template v-for="item, y in labelYMap">
					<text v-if="item.show"
							:x="plotOffsetLeft - tickSize / 2"
							:y="y"
							text-anchor="end"
							alignment-baseline="middle">
						{{ item.value }}
					</text>
					<line v-if="item.show" :x1="plotOffsetLeft - tickSize / 2" :x2="width - plotOffsetRight" :y1="y" :y2="y"></line>
				</template>
			</g>
			<!-- Print the points //-->
			<g class="irgraph-plot-items" v-hover-children="selected">
				<g v-for="item in series" class="irgraph-plot-item">
					<path :d="item.path" class="path" fill="transparent" />
					<line v-for="data in item.coords"
							:x1="data[0]"
							:y1="data[1]"
							:x2="data[0]"
							:y2="data[1]"
							class="circle" />
				</g>
			</g>
			<!-- Cursor //-->
			<g class="irgraph-plot-cursor">
				<line :x1="valueXToCoord(cursorXValue)" :x2="valueXToCoord(cursorXValue)" :y1="plotOffsetTop" :y2="plotOffsetTop + plotHeight"></line>
				<text class="irgraph-hide" ref="labelCursorXReferenceChar">M</text>
				<text :x="valueXToCoord(cursorXValue)"
						:y="plotOffsetTop + plotHeight + tickSize / 2 + labelCursorXCharSize.height"
						text-anchor="middle">
					{{ configProcessed.formatX(cursorXValue) }}
				</text>
			</g>
		</svg>
	</Element>
</template>

<script>
	"use strict";

	import Element from "./element.vue"
	import HoverChildren from "./directive/hover-children.js"
	import Resize from "[lib]/vue/directives/resize.js"

	const DEBUG = true;

	export default {
		components: {
			Element
		},
		props: {
			value: {type: Array, required: false, default: () => ([])},
			config: {type: Object, required: false, default: () => ({})}
		},
		directives: {
			"hover-children": HoverChildren,
			"resize": Resize
		},
		data: function() {
			return {
				selected: -1,
				width: 0,
				height: 0,
				valuesMinX: 0,
				valuesMinY: 0,
				valuesMaxX: 0,
				valuesMaxY: 0,
				plotOffsetTop: 10,
				plotOffsetRight: 10,
				//plotOffsetBottom: 20,
				tickSize: 5,
				labelXMaxNbChars: 6,
				labelYMaxNbChars: 6,
				tickYMinOffset: 50,
				maxPointsPerPlot: 20,
				cursorXValue: 0,
				/**
				 * Vertical offset to position the X label
				 */
				labelXCharSize: {
					width: 16,
					height: 16
				},
				/**
				 * Horizontal offset to position the Y label
				 */
				labelYCharSize: {
					width: 16,
					height: 16
				},
				/**
				 * Horizontal offset to position the Y label
				 */
				labelCursorXCharSize: {
					width: 16,
					height: 16
				}
			}
		},
		mounted() {
			{
				const rect = this.$refs.labelXReferenceChar.getBoundingClientRect();
				this.labelXCharSize = {
					height: rect.height || 16,
					width: rect.width || 16
				};
			}
			{
				const rect = this.$refs.labelYReferenceChar.getBoundingClientRect();
				this.labelYCharSize = {
					height: rect.height || 16,
					width: rect.width || 16
				};
			}
			{
				const rect = this.$refs.labelCursorXReferenceChar.getBoundingClientRect();
				this.labelCursorXCharSize = {
					height: rect.height || 16,
					width: rect.width || 16
				};
			}
		},
		watch: {
			value: {
				immediate: true,
				handler(value) {

					if (DEBUG) console.time("plot render")

					this.valuesMinY = Number.MAX_SAFE_INTEGER;
					this.valuesMaxY = -Number.MAX_SAFE_INTEGER;

					const series = value || [];

					// Calculate the [valuesMinY; valuesMaxY]
					this.sortedValues.forEach((array) => {
						array.forEach((point) => {
							this.valuesMinY = Math.min(this.valuesMinY, point[1]);
							this.valuesMaxY = Math.max(this.valuesMaxY, point[1]);
						});
					});

					// Calculate the [valuesMinX; valuesMaxX]
					if (this.sortedValues.length && this.sortedValues[0].length)
					{
						this.valuesMinX = Number.MAX_SAFE_INTEGER;
						this.valuesMaxX = -Number.MAX_SAFE_INTEGER;

						this.sortedValues.forEach((array) => {
							this.valuesMinX = Math.min(this.valuesMinX, array[0][0]);
							this.valuesMaxX = Math.max(this.valuesMaxX, array[array.length - 1][0]);
						});

						// Roughtly calculat the maximum numbers of characters for the labels
						this.labelXMaxNbChars = Math.max(this.configProcessed.formatX(this.valuesMinX).length,
								this.configProcessed.formatX(this.valuesMaxX).length);
						this.labelYMaxNbChars = Math.max(this.configProcessed.formatY(this.valuesMinY).length,
								this.configProcessed.formatY(this.valuesMaxY).length);
					}
					else {
						this.valuesMinX = 0;
						this.valuesMaxX = 1;
						this.labelXMaxNbChars = 6;
					}
				}
			}
		},
		computed: {
			/**
			 * SVG viewbox
			 */
			svgViewBox() {
				return "0 0 " + this.width + " " + this.height;
			},
			/**
			 * SVG style
			 */
			svgStyle() {
			//	return { width: "100%", height: "100%", overflow: "hidden" }
				return { width: this.width + "px", height: this.height + "px" }
			},
			configProcessed() {
				return Object.assign({
					minX: null,
					maxX: null,
					minY: null,
					maxY: null,
					/**
					 * Format the X labels
					 */
					formatX: (x) => Number(x).toFixed(this.labelXResolution),
					/**
					 * Format the Y labels
					 */
					formatY: (y) => Number(y).toFixed(this.labelYResolution),
					/**
					 * Overwrite the plotOffsetLeft value
					 */
					offsetLeft: null,
					/**
					 * Overwrite the plotOffsetBottom value
					 */
					offsetBottom: null
				}, this.config);
			},
			/**
			 * Full width of the plot area (excluding offsets)
			 */
			plotWidth() {
				return this.width - this.plotOffsetLeft - this.plotOffsetRight;
			},
			/**
			 * Full height of the plot area (excluding offsets)
			 */
			plotHeight() {
				return this.height - this.plotOffsetBottom - this.plotOffsetTop;
			},
			/**
			 * Space between the plot and the left
			 */
			plotOffsetLeft() {
				console.log(this.labelYMaxNbChars, this.labelYCharSize.width);
				return (this.configProcessed.offsetLeft === null) ? this.labelYMaxNbChars * this.labelYCharSize.width + this.tickSize / 2 : this.configProcessed.offsetLeft;
			},
			/**
			 * Space between the plot and the bottom
			 */
			plotOffsetBottom() {
				console.log(this.labelXCharSize, this.labelXCharSize.height + this.tickSize / 2);
				return (this.configProcessed.offsetBottom === null) ? this.labelXCharSize.height + this.tickSize / 2 : this.configProcessed.offsetBottom;
			},
			/**
			 * Minimum Y value to be displayed on the plot
			 */
			plotMinY() {
				if (this.configProcessed.minY === null) {
					// Note here the plotMaxY and valuesMinY are on purpose
					return ((this.plotMaxY - this.valuesMinY) > this.valuesMinY) ? Math.min(0, this.valuesMinY) : this.valuesMinY;
				}
				return this.configProcessed.minY;
			},
			/**
			 * Maximum Y value to be displayed on the plot
			 */
			plotMaxY() {
				return (this.configProcessed.maxY === null) ? this.valuesMaxY : this.configProcessed.maxY;
			},
			/**
			 * Minimum X value to be displayed on the plot
			 */
			plotMinX() {
				return (this.configProcessed.minX === null) ? this.valuesMinX : this.configProcessed.minX;
			},
			/**
			 * Maximum X value to be displayed on the plot
			 */
			plotMaxX() {
				return (this.configProcessed.maxX === null) ? this.valuesMaxX : this.configProcessed.maxX;
			},
			/**
			 * Pre-calculate ratio and offsets for x and y.
			 */
			valuesXRatio() {
				return 1. / (this.plotMaxX - this.plotMinX) * this.plotWidth;
			},
			valuesXOffset() {
				return this.plotOffsetLeft - this.plotMinX * this.valuesXRatio;
			},
			valuesYRatio() {
				return -1. / (this.plotMaxY - this.plotMinY) * this.plotHeight;
			},
			valuesYOffset() {
				return this.plotOffsetTop + this.plotHeight - this.plotMinY * this.valuesYRatio;
			},
			valuesValid() {
				return (this.valuesXRatio != Infinity) && (this.valuesYRatio != Infinity);
			},
			/**
			 * Values sorted by X. It should not give a high overhead on already sorted arrays.
			 */
			sortedValues() {
				return this.value.map((item) => {
					return item.values.sort((a, b) => {
						return a[0] - b[0];
					});
				});
			},
			/**
			 * The points to be processed
			 */
			series() {
				const series = this.value.map((item) => {

					let serie = {
						caption: item.caption || "",
						color: item.color || "red",
						coords: [],
						path: ""
					};

					// Prevent non-sense
					if (!this.valuesValid) {
						return serie;
					}

					// Display only a maximum number of entries to imprve performance
					const maxEntries = 100;
					const inc = Math.max(1, item.values.length / maxEntries);

					// Re-sample and calculate the series
					for (let i=0; i<item.values.length; i += inc) {
						const x = this.valuesXOffset + item.values[~~(i + .5)][0] * this.valuesXRatio;
						const y = this.valuesYOffset + item.values[~~(i + .5)][1] * this.valuesYRatio;
						serie.coords.push([x, y]);

						// Calculate the path
						if (i == 0)
						{
							serie.path = "M " + x + " " + y;
						}
						else
						{
							serie.path += " L " + x + " " + y;
							//item.path += " C " + x + " " + y + ", " + x + " " + y + ", " + x + " " + y;
						}
					}
					return serie;
				});
				if (DEBUG) console.timeEnd("plot render");
				return series;
			},
			// X Label
			labelX() {
				let maxNbTicks = Math.floor(this.plotWidth / this.labelXSpacing);
				const tickValueOffset = (this.plotMaxX - this.plotMinX) / maxNbTicks;
				let list = [];
				let value = this.plotMinX;
				while (maxNbTicks >= 0) {
					list.push(value);
					value += tickValueOffset;
					--maxNbTicks;
				}
				return list;
			},
			labelXResolution() {
				const incrementX = (this.plotMaxX - this.plotMinX) / this.labelX.length;
				let decimal = 0;
				for (let i = 1; i >= 0.00001; i /= 10, ++decimal) {
					if (incrementX >= i * 5) {
						return decimal;
					}
				}
				return decimal;
			},
			labelXMap() {
				let map = {};
				const lastIndex = this.labelX.length - 1;
				this.labelX.forEach((n, index) => {
					map[this.plotOffsetLeft + this.plotWidth * index / lastIndex] = {
						value: this.configProcessed.formatX(n),
						show: (index > 0 && index < lastIndex)
					};
				});
				return map;
			},
			/**
			 * Spacing beteen X labels
			 */
			labelXSpacing() {
				return this.labelXMaxNbChars * this.labelXCharSize.width;
			},
			// Y Label
			labelY() {
				let maxNbTicks = Math.floor(this.plotHeight / this.tickYMinOffset);
				const tickValueOffset = (this.plotMaxY - this.plotMinY) / maxNbTicks;
				let list = [];
				let value = this.plotMinY;
				while (maxNbTicks >= 0) {
					list.push(value);
					value += tickValueOffset;
					--maxNbTicks;
				}
				return list;
			},
			labelYResolution() {
				const incrementY = (this.plotMaxY - this.plotMinY) / this.labelY.length;
				let decimal = 0;
				for (let i = 1; i >= 0.00001; i /= 10, ++decimal) {
					if (incrementY >= i * 5) {
						return decimal;
					}
				}
				return decimal;
			},
			labelYMap() {
				let map = {};
				const lastIndex = this.labelY.length - 1;
				this.labelY.reverse().forEach((n, index) => {
					map[this.plotOffsetTop + this.plotHeight * index / lastIndex] = {
						value: this.configProcessed.formatY(n),
						show: true
					};
				});
				return map;
			}
		},
		methods: {
			handleResize(width, height) {
				const rect = this.$refs.canvas.parentElement.getBoundingClientRect();
				this.width = rect.width;
				this.height = rect.height;
			},
			/**
			 * Return the closest index in the sorted array to x
			 */
			findClosestX(values, x) {
				let index = 0;
				let indexEnd = values.length - 1;

				while (index <= indexEnd) {
					const indexCurrent = Math.floor((index + indexEnd) / 2);
					const xCurrent = values[indexCurrent][0];
					if (xCurrent == x) {
						index = indexCurrent;
						break;
					}
					else if (xCurrent < x) {
						index = indexCurrent + 1;
					}
					else {
						indexEnd = indexCurrent - 1;
					}
				}

				if (index <= 0) return 0;
				if (index >= values.length) return values.length - 1;

				return (x - values[index - 1][0] < values[index][0] - x) ? index - 1 : index;
			},
			coordXToValue(x) {
				return (x - this.valuesXOffset) / this.valuesXRatio;
			},
			valueXToCoord(x) {
				return x * this.valuesXRatio + this.valuesXOffset;
			},
			handleMouseMove(e) {

				const rect = this.$refs.canvas.getBoundingClientRect();
				const coords = (e.touches && e.touches.length) ? {x: e.touches[0].pageX - rect.left, y: e.touches[0].pageY - rect.top} : {x: e.pageX - rect.left, y: e.pageY - rect.top};
				const valueX = this.coordXToValue(coords.x);

				// Look for the closest match
				const positions = this.sortedValues.map((array) => this.findClosestX(array, valueX));
				const distances = positions.map((pos, index) => Math.abs(this.sortedValues[index][pos][0] - valueX));
				const index = distances.reduce((iMin, x, i, arr) => ((x < arr[iMin]) ? i : iMin), 0);

				this.cursorXValue = this.sortedValues[index][positions[index]][0];
			}
		}
	}
</script>
