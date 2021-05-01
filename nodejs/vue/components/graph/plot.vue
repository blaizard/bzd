<template>
	<Element
		name="irgraph-plot"
		:value="series"
		:config="config"
		:selected="selected"
		v-slot:default="slotProps"
		v-resize="handleResize">
		<svg
			ref="canvas"
			:viewBox="svgViewBox"
			:style="svgStyle"
			@mousemove.stop="handleMouseMove"
			@mouseleave.stop="handleMouseLeave">
			<g class="irgraph-plot-labels-y" v-show="configProcessed.showAxisY">
				<text class="irgraph-hide" ref="labelYReferenceChar">0</text>
				<template v-for="(item, y) in labelYMap">
					<PlotText
						v-if="item.show"
						:x="plotOffsetLeft - tickSize / 2"
						:y="y - labelYCharSize.height / 2"
						h-align="right">
						{{ item.value }}
					</PlotText>
					<line
						v-if="item.show"
						:x1="plotOffsetLeft - tickSize / 2"
						:x2="width - plotOffsetRight"
						:y1="y"
						:y2="y"></line>
				</template>
			</g>
			<g class="irgraph-plot-labels-x" v-show="configProcessed.showAxisX">
				<text class="irgraph-hide" ref="labelXReferenceChar">0</text>
				<template v-for="(item, x) in labelXMap">
					<PlotText v-if="item.show" :x="x" :y="height - plotOffsetBottom + tickSize / 2" h-align="middle">
						{{ item.value }}
					</PlotText>
					<line
						v-if="item.show"
						:x1="x"
						:x2="x"
						:y1="height - plotOffsetBottom - tickSize / 2"
						:y2="height - plotOffsetBottom + tickSize / 2"></line>
				</template>
				<line
					class="irgraph-plot-axis-x"
					:x1="plotOffsetLeft"
					:x2="width - plotOffsetRight"
					:y1="height - plotOffsetBottom"
					:y2="height - plotOffsetBottom"></line>
			</g>
			<!-- Print the points //-->
			<g class="irgraph-plot-items">
				<!-- v-hover-children="selected"> //-->
				<component
					v-for="(serie, index) in series"
					:class="getItemClass(index)"
					:style="getItemStyle(index, serie)"
					:key="index"
					:is="getSerieComponent(serie)"
					:selected="index == selected ? selectedPoint : -1"
					:serie="serie"
					:bounding-box="plotBoundingBox"></component>
			</g>
			<!-- Cursor //-->
			<g class="irgraph-plot-cursor" v-show="configProcessed.showCursor">
				<line
					:x1="valueXToCoord(cursorXValue)"
					:x2="valueXToCoord(cursorXValue)"
					:y1="plotOffsetTop"
					:y2="plotOffsetTop + plotHeight"></line>
				<text class="irgraph-hide" ref="labelCursorXReferenceChar">0</text>
				<PlotText :x="valueXToCoord(cursorXValue)" :y="plotOffsetTop + plotHeight + tickSize / 2" h-align="middle">
					{{ configProcessed.formatX(cursorXValue) }}
				</PlotText>
			</g>
		</svg>
	</Element>
</template>

<script>
	import Element from "./element.vue";
	import PlotText from "./utils/text.vue";
	import HoverChildren from "./directive/hover_children.mjs";
	import Resize from "../../directives/resize.mjs";
	import { tooltip } from "../../directives/tooltip.mjs";
	import RenderLine from "./render/line.vue";
	import RenderBar from "./render/bar.vue";

	import Colors from "bzd-style/css/colors.scss";

	const DEBUG = true;

	export default {
		components: {
			Element,
			PlotText,
			RenderLine
		},
		props: {
			value: { type: Array, required: false, default: () => [] },
			config: { type: Object, required: false, default: () => ({}) }
		},
		directives: {
			"hover-children": HoverChildren,
			resize: Resize
		},
		data: function() {
			return {
				selected: -1,
				selectedPoint: -1,
				width: 0,
				height: 0,
				valuesMinX: 0,
				valuesMinY: 0,
				valuesMaxX: 0,
				valuesMaxY: 0,
				tickSize: 5,
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
			};
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
				handler(/*value*/) {
					if (DEBUG) {
						console.time("plot render");
					}

					let valuesMinY = Number.MAX_SAFE_INTEGER;
					let valuesMaxY = -Number.MAX_SAFE_INTEGER;
					let valuesMinX = Number.MAX_SAFE_INTEGER;
					let valuesMaxX = -Number.MAX_SAFE_INTEGER;

					// Calculate the [valuesMinY; valuesMaxY]
					this.valueSorted.forEach((array) => {
						array.values.forEach((point) => {
							valuesMinY = Math.min(valuesMinY, point[1]);
							valuesMaxY = Math.max(valuesMaxY, point[1]);
						});
					});

					// Calculate the [valuesMinX; valuesMaxX]
					if (this.valueSorted.length && this.valueSorted[0].values.length) {
						this.valueSorted.forEach((array) => {
							valuesMinX = Math.min(valuesMinX, array.values[0][0]);
							valuesMaxX = Math.max(valuesMaxX, array.values[array.values.length - 1][0]);
						});
					}
					else {
						valuesMinX = 0;
						valuesMaxX = 1;
					}

					// Assign the values
					this.valuesMinY = valuesMinY;
					this.valuesMaxY = valuesMaxY;
					this.valuesMinX = valuesMinX;
					this.valuesMaxX = valuesMaxX;
				}
			}
		},
		computed: {
			presetColors() {
				return Colors;
			},
			colorsList() {
				return Object.values(this.presetColors);
			},
			labelYMaxNbChars() {
				return Math.max(
					this.configProcessed.formatY(this.valuesMinY).length,
					this.configProcessed.formatY(this.valuesMaxY).length
				);
			},
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
				return { width: this.width + "px", height: this.height + "px" };
			},
			configProcessed() {
				return Object.assign(
					{
						minX: null,
						maxX: null,
						minY: null,
						maxY: null,
						/**
						 * Max entries to be displayed at a time per series
						 */
						maxEntries: 100,
						/**
						 * Format the X labels
						 */
						formatX: (x) => String(Number(x)),
						/**
						 * Format the Y labels
						 */
						formatY: (y) => Number(y).toFixed(this.labelYMaxResolution),
						/**
						 * Add extra offset to the left
						 */
						paddingLeft: 0,
						/**
						 * Add extra offset to the top
						 */
						paddingTop: 0,
						/**
						 * Add extra offset to the right
						 */
						paddingRight: 0,
						/**
						 * Add extra offset to the bottom
						 */
						paddingBottom: 0,
						/**
						 * Show X axis
						 */
						showAxisX: true,
						/**
						 * Show Y axis
						 */
						showAxisY: true,
						/**
						 * Show the cursor
						 */
						showCursor: true
					},
					this.config
				);
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
			 * Space between the plot and the right
			 */
			plotOffsetRight() {
				return this.configProcessed.paddingRight;
			},
			/**
			 * Space between the plot and the top
			 */
			plotOffsetTop() {
				let value = this.configProcessed.paddingTop;
				if (this.configProcessed.showAxisY) {
					value = Math.max(value, this.labelXCharSize.height / 2);
				}
				return value;
			},
			/**
			 * Space between the plot and the left
			 */
			plotOffsetLeft() {
				let value = 0;
				if (this.configProcessed.showAxisY) {
					value = this.labelYMaxNbChars * this.labelYCharSize.width + this.tickSize / 2;
				}
				value += this.configProcessed.paddingLeft;
				return value;
			},
			/**
			 * Space between the plot and the bottom
			 */
			plotOffsetBottom() {
				let value = 0;
				if (this.configProcessed.showAxisY) {
					value = this.labelXCharSize.height / 2;
				}
				if (this.configProcessed.showAxisX) {
					value = Math.max(value, this.labelXCharSize.height + this.tickSize / 2);
				}
				if (this.configProcessed.showCursor) {
					value = Math.max(value, this.labelCursorXCharSize.height + this.tickSize / 2);
				}
				value += this.configProcessed.paddingBottom;
				return value;
			},
			/**
			 * Minimum Y value to be displayed on the plot
			 */
			plotMinY() {
				if (this.configProcessed.minY === null) {
					// Note here the plotMaxY and valuesMinY are on purpose
					return this.plotMaxY - this.valuesMinY > this.valuesMinY ? Math.min(0, this.valuesMinY) : this.valuesMinY;
				}
				return this.configProcessed.minY;
			},
			/**
			 * Maximum Y value to be displayed on the plot
			 */
			plotMaxY() {
				return this.configProcessed.maxY === null ? this.valuesMaxY : this.configProcessed.maxY;
			},
			/**
			 * Minimum X value to be displayed on the plot
			 */
			plotMinX() {
				return this.configProcessed.minX === null ? this.valuesMinX : this.configProcessed.minX;
			},
			/**
			 * Maximum X value to be displayed on the plot
			 */
			plotMaxX() {
				return this.configProcessed.maxX === null ? this.valuesMaxX : this.configProcessed.maxX;
			},
			/**
			 * Return the coordinates (top, left, bottom, right) of the plot bounding box
			 */
			plotBoundingBox() {
				return {
					top: this.plotOffsetTop,
					left: this.plotOffsetLeft,
					bottom: this.height - this.plotOffsetBottom,
					right: this.width - this.plotOffsetRight
				};
			},
			/**
			 * Pre-calculate ratio and offsets for x and y.
			 */
			valuesXRatio() {
				return (1 / (this.plotMaxX - this.plotMinX)) * this.plotWidth;
			},
			valuesXOffset() {
				return this.plotOffsetLeft - this.plotMinX * this.valuesXRatio;
			},
			valuesYRatio() {
				return (-1 / (this.plotMaxY - this.plotMinY)) * this.plotHeight;
			},
			valuesYOffset() {
				return this.plotOffsetTop + this.plotHeight - this.plotMinY * this.valuesYRatio;
			},
			valuesValid() {
				return this.valuesXRatio != Infinity && this.valuesYRatio != Infinity;
			},
			/**
			 * Values sorted by X. It should not give a high overhead on already sorted arrays.
			 */
			valueSorted() {
				return this.value.map((item) => {
					let copyItem = { ...item };
					copyItem.values = [...copyItem.values];
					copyItem.values.sort((a, b) => {
						return a[0] - b[0];
					});
					return copyItem;
				});
			},
			/**
			 * The points to be processed
			 */
			series() {
				const series = this.valueSorted.map((item, index) => {
					const tooltipFormater = item.tooltip || ((x, y) => "Value: " + y);

					let serie = {
						caption: item.caption || "",
						color: this.selectColorValue(index, item.color),
						type: item.type || "line",
						coords: [],
						values: [],
						path: "",
						tooltip: (elt) => {
							if (index == this.selected) {
								const value = serie.values[this.selectedPoint];
								tooltip(elt, {
									type: "html",
									data: tooltipFormater(value[0], value[1])
								});
							}
						}
					};

					// Prevent non-sense
					if (!this.valuesValid) {
						return serie;
					}

					// Display only a maximum number of entries to improve performance
					const inc = Math.max(1, item.values.length / this.configProcessed.maxEntries);

					// Re-sample and calculate the series
					for (let i = 0; i < item.values.length; i += inc) {
						const valueX = item.values[~~(i + 0.5)][0];
						const valueY = item.values[~~(i + 0.5)][1];
						const x = this.valuesXOffset + valueX * this.valuesXRatio;
						const y = this.valuesYOffset + valueY * this.valuesYRatio;
						serie.coords.push([x, y]);
						serie.values.push([valueX, valueY]);
					}
					return serie;
				});
				if (DEBUG) {
					console.timeEnd("plot render");
				}
				return series;
			},
			// X Label
			labelX() {
				let list = [];
				let value = this.plotMinX;
				while (value < this.plotMaxX) {
					list.push(value);
					value += this.labelXSpacingValue;
				}
				return list;
			},
			labelXMaxResolution() {
				// We don't want to depend on this.labelX.length here, otherwise we end up in a loop
				const incrementX = (this.plotMaxX - this.plotMinX) / 100;
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
					map[this.plotOffsetLeft + (this.plotWidth * index) / lastIndex] = {
						value: this.configProcessed.formatX(n),
						show: index > 0 && index < lastIndex
					};
				});
				return map;
			},
			/**
			 * Spacing between X labels
			 */
			labelXSpacingValue() {
				const defaultSpacingValue = (5 * this.labelXCharSize.width) / this.valuesXRatio;
				const log = Math.log10(defaultSpacingValue);
				const minValue = Math.pow(10, Math.floor(log)) * 5;

				// Ensure that the labelXMaxResolution
				return defaultSpacingValue > minValue ? minValue * 2 : minValue;
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
			labelYMaxResolution() {
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
				if (lastIndex) {
					[...this.labelY].reverse().forEach((n, index) => {
						map[this.plotOffsetTop + (this.plotHeight * index) / lastIndex] = {
							value: this.configProcessed.formatY(n),
							show: true
						};
					});
				}
				return map;
			}
		},
		methods: {
			selectColorValue(index, color) {
				if (color in this.presetColors) {
					return this.presetColors[color];
				}
				if (color) {
					return color;
				}
				return this.colorsList[index % this.colorsList.length];
			},
			getItemClass(index) {
				return {
					"irgraph-plot-item": true,
					"irgraph-plot-item-selected": this.selected == -1 ? true : this.selected == index
				};
			},
			getItemStyle(index, serie) {
				return "--bzd-graph-color: " + serie.color + ";";
			},
			/**
			 * Get the component associated with a serie, based on its type
			 */
			getSerieComponent(serie) {
				switch (serie.type) {
				case "line":
					return RenderLine;
				case "bar":
					return RenderBar;
				}
			},
			handleResize(/*width, height*/) {
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

				if (index <= 0) {
					return 0;
				}
				if (index >= values.length) {
					return values.length - 1;
				}

				return x - values[index - 1][0] < values[index][0] - x ? index - 1 : index;
			},
			coordXToValue(x) {
				return (x - this.valuesXOffset) / this.valuesXRatio;
			},
			coordYToValue(y) {
				return (y - this.valuesYOffset) / this.valuesYRatio;
			},
			valueXToCoord(x) {
				return x * this.valuesXRatio + this.valuesXOffset;
			},
			handleMouseMove(e) {
				const rect = this.$refs.canvas.getBoundingClientRect();
				const coords =
					e.touches && e.touches.length
						? { x: e.touches[0].pageX - rect.left, y: e.touches[0].pageY - rect.top }
						: { x: e.pageX - rect.left, y: e.pageY - rect.top };
				const valueX = this.coordXToValue(coords.x);
				const valueY = this.coordYToValue(coords.y);

				// Look for the closest match
				const positions = this.valueSorted.map((array) => this.findClosestX(array.values, valueX));
				const distances = positions.map((pos, index) => {
					const distX = this.valueSorted[index].values[pos][0] - valueX;
					const distY = this.valueSorted[index].values[pos][1] - valueY;
					return distX * distX + distY * distY;
				});
				const index = distances.reduce((iMin, x, i, arr) => (x < arr[iMin] ? i : iMin), 0);

				this.cursorXValue = this.valueSorted[index].values[positions[index]][0];

				this.selected = index;
				this.selectedPoint = positions[index];
			},
			handleMouseLeave() {
				this.selected = -1;
				this.selectedPoint = -1;
			}
		}
	};
</script>

<style lang="scss">
	@use "bzd-style/css/graph/plot.scss";
</style>
