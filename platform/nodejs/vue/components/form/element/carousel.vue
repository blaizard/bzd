<template>
	<div class="irform-carousel">
		<!-- Important to use v-show here as the transition animation will be affected
		with v-if as a redraw is involved //-->
		<div v-show="isControlPrevious" class="irform-carousel-prev" @click="slidePrevious">&lt;</div>
		<div class="irform-carousel-container"
				:tabindex="(disable) ? undefined : 0"
				ref="container"
				@focus="setActive()"
				@blur="setInactive()"
				@keydown="handleKeyDown($event)"
				v-touch="touchDirective">
			<div class="irform-carousel-slides"
					ref="slides"
					:style="slidesStyle">
				<div v-for="(slide, index) in list"
						:class="getSlideClass(index)"
						:style="slideStyle"
						@click="handleClick(index)">
					<div class="irform-carousel-slide-container" v-html="slide.display"></div>
				</div>
			</div>
		</div>
		<div v-show="isControlNext" class="irform-carousel-next" @click="slideNext">&gt;</div>
	</div>
</template>

<script>
	"use strict";

	import Element from "./element.vue"
	import Touch from "[lib]/vue/directives/touch.js"

	export default {
		mixins: [Element],
		directives: {
			touch: Touch
		},
		props: {
			value: {type: String | Number, required: false, default: ""}
		},
		mounted() {
			this.fetchList(this.getOption("list", []));

			// Recalculate the position as if the slide geometry changed,
			// its alignement will be different. This can happen while loading images
			// or other asynchronous operations.
			if (typeof ResizeObserver !== "undefined") {
				const myObserver = new ResizeObserver((entries) => {
					this.computeSlidePosition();
				});
				myObserver.observe(this.$refs.slides);
			}
			else {
				this.interval = setInterval(this.computeSlidePosition, 1000);
			}
		},
		beforeDestroy() {
			if (this.interval) {
				clearInterval(this.interval);
			}
		},
		data: function() {
			return {
				/**
				 * Defines how the selected items aligns with the slider.
				 * Values are "left", "center", "right" or false. False means that the selection
				 * will not be aligned and slecting a new slide will not affect the scrolling
				 * of the slider
				 */
				alignSelection: this.getOption("alignSelection", "center"),
				/**
				 * Maximum number of slides visible at a time. If set to 0, all the slides will be collpased.
				 */
				maxVisible: this.getOption("maxVisible", 0),
				/**
				 * Margin between the slides
				 */
				margin: this.getOption("margin", 0),
				/**
				 * Print controls. If auto, it will be print them only if necessary, it "none", it will nto print them,
				 * if "always", it will always print them.
				 */
				controls: this.getOption("controls", "auto"),
				/**
				 * Callback when an element is clicked
				 */
				click: this.getOption("click", (index) => {}),
				// ---- Internal ----
				// Current slide selected
				index: 0,
				interval: null,
				slidePosition: 0,
				offsetX: 0,
				isDrag: false,
				list: []
			}
		},
		watch: {
			value: function(value) {
				this.slideSelectByValue(value);
			}
		},
		computed: {
			touchDirective() {
				return (this.disable) ? {} : {
					onswipe: this.onSwipe,
					ondrag: this.onDrag,
					onstopdrag: this.onStopDrag,
					allowClickThrough: true
				};
			},
			slidesStyle() {
				return {
					position: "relative",
					transform: "translate(" + (this.slidePosition + this.offsetX) + "px)",
					marginLeft: (this.alignSelection == "center") ? "50%" : ((this.alignSelection == "right") ? "100%" : 0),
					// Disable transitions while dragging
					transition: (this.isDrag) ? "none" : undefined
				}
			},
			slideStyle() {
				return {
					margin: this.margin,
					minWidth: (this.maxVisible) ? (Math.round(100 / this.maxVisible) + "%") : "auto"
				}
			},
			isControlPrevious() {
				if (this.controls == "always") {
					return true;
				}
				if (this.controls == "none") {
					return false;
				}
				return (this.index - 1  >= 0);
			},
			isControlNext() {
				if (this.controls == "always") {
					return true;
				}
				if (this.controls == "none") {
					return false;
				}
				return (this.index + 1 < this.list.length);
			}
		},
		methods: {
			onSwipe(direction, speedPixelPerSecond) {
				
				const nbExtraSlides = Math.floor(speedPixelPerSecond * speedPixelPerSecond / (400 * 400));
				const rectCurSlide = this.$refs.slides.children[this.index].getBoundingClientRect();
				let curSlideWidth = rectCurSlide.width;

				switch (direction) {

				case "right":
					// Identify the slide currently selected
					for (let slidesWidth = 0; this.index > 0 && slidesWidth + curSlideWidth / 2 < this.offsetX; this.index--) {
						const rect = this.$refs.slides.children[this.index - 1].getBoundingClientRect();
						curSlideWidth = rect.width;
						slidesWidth += curSlideWidth;
					}
					this.slideSelectByIndex(this.index - nbExtraSlides);
					break;

				case "left":
					// Identify the slide currently selected
					const rect = this.$refs.slides.children[this.index].getBoundingClientRect();
					for (let totalSlideWidth = 0; this.index < this.list.length - 1 && totalSlideWidth + curSlideWidth / 2 < -this.offsetX; this.index++) {
						const rect = this.$refs.slides.children[this.index + 1].getBoundingClientRect();
						curSlideWidth = rect.width;
						totalSlideWidth += curSlideWidth;
					}
					this.slideSelectByIndex(this.index + nbExtraSlides);
					break;
				}
			},
			onDrag(x, y) {
				this.isDrag = true;
				this.offsetX = x;
			},
			onStopDrag() {
				this.isDrag = false;
				this.offsetX = 0;
			},
			async fetchList(listOrFct) {
				const list = (typeof listOrFct === "function") ? await listOrFct() : listOrFct;
				this.list = this.createMultiValueList(list, /*isHtml*/true);

				// Select the right slide
				this.slideSelectByValue(this.get());

				// Refresh the coordinates
				this.$nextTick(this.computeSlidePosition);
			},
			getSlideClass(index) {
				return {
					"irform-carousel-slide": true,
					"irform-carousel-slide-selected": (index == this.index)
				}
			},
			slidePrevious() {
				this.slideSelectByIndex(this.index - 1);
			},
			slideNext() {
				this.slideSelectByIndex(this.index + 1);
			},
			slideSelectByIndex(index) {
				// Make sure the value is in the boundaries
				index = Math.max(Math.min(index, this.list.length - 1), 0);

				this.index = index;
				this.computeSlidePosition();

				// Make the element active if it was not
				if (!this.isActive) {
					this.$refs.container.focus();
				}

				this.set(this.list[index].value);
			},
			slideSelectByValue(value) {
				if (this.list.length && this.list[this.index].value != value) {
					for (let index = 0; index < this.list.length; ++index) {
						if (this.list[index].value == value) {
							this.index = index;
							break;
						}
					}
				}
			},
			computeSlidePosition() {
				if (this.$refs.slides && this.$refs.slides.children && this.$refs.slides.children[this.index]) {
					const rectBase = this.$refs.slides.getBoundingClientRect();
					const rect = this.$refs.slides.children[this.index].getBoundingClientRect();

					switch (this.alignSelection) {
					case "center":
						this.slidePosition = Math.round(rectBase.left - rect.left - rect.width / 2);
						break;
					case "right":
						this.slidePosition = Math.round(rectBase.left - rect.left - rect.width);
						break;
					case "left":
						this.slidePosition = Math.round(rectBase.left - rect.left);
						break;
					default:
						const rectContainer = this.$refs.container.getBoundingClientRect();
						// if the selection is too much on the left (not visible)
						if (rect.left - rectContainer.left < 0) {
							this.slidePosition = Math.round(rectBase.left - rect.left);
						}
						// if the selection is too much on the right (not visible)
						else if (rect.right - rectContainer.left > rectContainer.width) {
							this.slidePosition = Math.round(rectBase.left - rect.right + rectContainer.width);
						}
						// Add the current offset to the slide position as this will be reset right after.
						// This needs to be done here are we do not want to affect the slidePosition when
						// it reaches corners cases (see above).
						else {
							this.slidePosition += this.offsetX;
						}
						// Do not let the slides too much on the right (useless)
						this.slidePosition = Math.min(0, this.slidePosition);
					}
				}
				else {
					this.slidePosition = 0;
				}
			},
			handleClick(index) {
				if (!this.disable) {
					this.click(index);
					this.slideSelectByIndex(index);
				}
			},
			handleKeyDown(e) {
				if (e.keyCode == /*arrow left*/37) {
					this.slidePrevious();
				}
				else if (e.keyCode == /*arrow right*/39) {
					this.slideNext();
				}
			}
		}
	}
</script>
