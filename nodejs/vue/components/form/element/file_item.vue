<template>
	<div :class="containerClass">
		<!-- Error message //-->
		<div class="irform-file-error" v-if="isError">{{ errorMessage }}</div>

		<!-- Image/Icon //-->
		<span class="irform-file-icon">
			<div v-if="isLoading">...</div>
			<img
				v-else-if="isImage"
				:src="imageSrc"
				draggable="false"
				ondragstart="return false;"
				v-touch="touchDirective"
				:style="styleImage"
			/>
			<i v-else class="bzd-icon-file"></i>
		</span>

		<!-- Name //-->
		<div v-if="isFile" class="irform-file-name">{{ path }}</div>
		<!--<div v-else-if="isUpload" class="irform-file-name">{{ fileName }} ({{ currentSizeStr }})</div>//-->

		<!-- Actions //-->
		<div class="irform-file-actions">
			<div v-if="templateEdit && isImage" class="irform-file-zoom-in" @click.stop="handleZoomIn">+</div>
			<div v-if="templateEdit && isImage" class="irform-file-zoom-out" @click.stop="handleZoomOut">-</div>
		</div>
	</div>
</template>

<script>
	import Element from "./element.vue";
	import Touch from "../../../directives/touch.mjs";

	export default {
		mixins: [Element],
		directives: {
			touch: Touch,
		},
		emits: ["delete", "update:model-value"],
		data: function () {
			return {
				error: null,
				isLoading: false,
				allowDelete: true,
				templateEdit: false,
				imageFill: "cover",
				imageEdit: {
					containerWidth: 0,
					containerHeight: 0,
					x: 0,
					y: 0,
					width: 0,
					height: 0,
				},
				current: {
					x: 0,
					y: 0,
				},
			};
		},
		computed: {
			containerClass() {
				return {
					"irform-file": true,
					"irform-file-image": this.isImage,
					"irform-file-file": this.isFile,
					"irform-file-upload": this.isUpload,
					"irform-error": this.isError,
					//[this.templateClass]: true,
				};
			},
			value() {
				return this.get() || {};
			},
			path() {
				return this.value.path;
			},
			isError() {
				return Boolean(this.errorMessage);
			},
			errorMessage() {
				return this.value.error || this.error;
			},
			isAvailable() {
				return typeof this.path === "string";
			},
			isImage() {
				return this.isAvailable && Boolean(this.value.imageUrl);
			},
			isFile() {
				return this.isAvailable && !this.isImage;
			},
			isUpload() {
				return !this.isAvailable;
			},
			styleImage() {
				return (
					"left: " +
					(this.imageEdit.x + this.current.x) +
					"px; top: " +
					(this.imageEdit.y + this.current.y) +
					"px; width: " +
					this.imageEdit.width +
					"px; height: " +
					this.imageEdit.height +
					"px;"
				);
			},
			touchDirective() {
				return {
					enable: this.templateEdit,
					nop: false,
					ondrag: (x, y) => {
						this.current.x = x;
						this.current.y = y;
					},
					onstopdrag: () => {
						this.imageEdit.x += this.current.x;
						this.imageEdit.y += this.current.y;
						this.current.x = 0;
						this.current.y = 0;
						this.updateValue();
					},
				};
			},
		},
		methods: {
			handleZoomIn() {
				this.imageEdit.width *= 1.1;
				this.imageEdit.height *= 1.1;
				this.updateValue();
			},
			handleZoomOut() {
				this.imageEdit.width /= 1.1;
				this.imageEdit.height /= 1.1;
				this.updateValue();
			},
			updateValue() {
				const value = Object.assign(
					{
						path: this.path,
					},
					this.imageEdit,
				);
				this.$emit("update:model-value", value);
			},
		},
	};
</script>

<style lang="scss" scoped>
	.irform-file-image {
		position: relative;
		img {
			position: absolute;
		}
	}
</style>
