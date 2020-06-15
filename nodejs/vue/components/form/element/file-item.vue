<template>
	<div :class="containerClass">
		<!-- Error message //-->
		<div class="irform-file-error" v-if="isError">{{ errorMessage }}</div>

		<!-- Image //-->
		<div v-if="isLoading">...</div>
		<img v-else-if="isImage" :src="imgSrc" draggable="false" ondragstart="return false;" v-touch="touchDirective" :style="styleImage" />
		<i v-else class="irform-icon-file"></i>

		<!-- Name //-->
		<div v-if="isFile" class="irform-file-name">{{ fileName }}</div>
		<div v-else-if="isUpload" class="irform-file-name">{{ fileName }} ({{ currentSizeStr }})</div>

		<!-- Progress bar //-->
		<div v-if="isUpload && !isError" class="irform-file-progress">
			<div class="irform-file-progress-bar" :style="{width: Math.round(this.progress * 100) + '%'}"></div>
		</div>

		<!-- Delete button //-->
		<div class="irform-file-actions">
			<div v-if="templateEdit && isImage" class="irform-file-zoom-in" @click.stop="handleZoomIn">+</div>
			<div v-if="templateEdit && isImage" class="irform-file-zoom-out" @click.stop="handleZoomOut">-</div>
			<div v-if="allowDelete" class="irform-file-delete" @click.stop="$emit('delete')">x</div>
		</div>
	</div>
</template>

<script>
import Touch from "../../../directives/touch.mjs";

export default {
	props: {
		value: {type: Object | String, required: true, default: () => ({})},
		disable: {type: Boolean, required: false, default: false},
		config: {type: Object, required: true, default: () => ({})}
	},
	directives: {
		touch: Touch
	},
	data: function() {
		return {
			allowDelete: true,
			error: null,
			isLoading: false,
			imgSrc: null,
			imageUrl: null,
			templateClass: "",
			templateEdit: false,
			imageEdit: {
				x: 0,
				y: 0,
				scale: 1
			}
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
				[this.templateClass]: true
			};
		},
		touchDirective() {
			return {
				enable: this.templateEdit,
				nop: false,
				ondrag: (x, y) => {
					this.imageEdit.x = x;
					this.imageEdit.y = y;
				}
			};
		},
		styleImage() {
			return "left: " + this.imageEdit.x + "px; top: " + this.imageEdit.y + "px; transform: scale(" + this.imageEdit.scale + ");";
		},
		isAvailable() {
			return (typeof this.value === "string");
		},
		isError() {
			return (!this.isAvailable && this.value.error) || this.error;
		},
		errorMessage() {
			return this.value.error || this.error;
		},
		isImage() {
			return this.isAvailable && this.imageUrl;
		},
		isFile() {
			return this.isAvailable && !this.isImage;
		},
		isUpload() {
			return !this.isAvailable;
		},
		progress() {
			return Math.min(this.value.currentBytes / this.value.totalBytes, 1);
		},
		currentSizeStr() {
			if (!("currentBytes" in this.value) || !this.value.currentBytes) {
				return "-";
			}
			let size = this.value.currentBytes;
			let unitIndex = 0;
			const unitList = ["B", "KB", "MB", "GB", "TB"];
			while (size > 768) {
				size /= 1024;
				unitIndex++;
			}
			return String(Math.round(size * 100) / 100) + unitList[unitIndex];
		},
		fileName() {
			const path = (this.isAvailable) ? this.value : this.value.fileName;
			return path.split("/").pop();
		}
	},
	watch: {
		isImage: {
			/*
			 * Important to ensure that it is triggered even if the value does not change
			 * (is already set to true at the begining)
			 */
			immediate: true,
			handler: function () {
				if (this.isImage) {
					this.imagePreload(this.imageUrl);
				}
			}
		},
		value: {
			immediate: true,
			handler: async function (value) {
				this.imageUrl = null;
				if (this.isAvailable) {
					if (typeof this.config.imageToUrl === "function") {
						this.imageUrl = await this.config.imageToUrl(value);
					}
					else if (typeof this.config.imageToUrl === "string") {
						this.imageUrl = this.config.imageToUrl;
					}
				}
			}
		},
	},
	methods: {
		handleZoomIn() {
			this.imageEdit.scale *= 1.1;
		},
		handleZoomOut() {
			this.imageEdit.scale /= 1.1;
		},
		imagePreload(url) {
			let imagePreload = new Image();
			this.isLoading = true;
			imagePreload.onload = () => {
				this.imgSrc = imagePreload.src;
				this.isLoading = false;
			};
			imagePreload.onerror = () => {
				this.error = "Cannot load image";
				this.isLoading = false;
			};
			imagePreload.src = url;
		}
	}
};
</script>

<style lang="scss" scoped>
	.irform-file-image {
		img {
			position: relative;
		}
	}
</style>
