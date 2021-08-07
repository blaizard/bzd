<template>
	<div :class="containerClass">
		<!-- Error message //-->
		<div class="irform-file-error" v-if="isError">{{ errorMessage }}</div>

		<!-- Image //-->
		<div v-if="isLoading">...</div>
		<img
			v-else-if="isImage"
			:src="imageSrc"
			draggable="false"
			ondragstart="return false;"
			v-touch="touchDirective"
			:style="styleImage"/>
		<i v-else class="irform-icon-file"></i>

		<!-- Name //-->
		<div v-if="isFile" class="irform-file-name">{{ fileName }}</div>
		<div v-else-if="isUpload" class="irform-file-name">{{ fileName }} ({{ currentSizeStr }})</div>

		<!-- Progress bar //-->
		<div v-if="isUpload && !isError" class="irform-file-progress">
			<div class="irform-file-progress-bar" :style="{ width: Math.round(this.progress * 100) + '%' }"></div>
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
			value: { type: Object | String, required: true, default: () => ({}) },
			disable: { type: Boolean, required: false, default: false },
			config: { type: Object, required: true, default: () => ({}) }
		},
		directives: {
			touch: Touch
		},
		data: function() {
			return {
				allowDelete: true,
				error: null,
				isLoading: false,
				templateClass: "",
				templateEdit: false,
				imageSrc: null,
				imageUrl: null,
				imageFill: "cover",
				imageEdit: {
					containerWidth: 0,
					containerHeight: 0,
					x: 0,
					y: 0,
					width: 0,
					height: 0
				},
				current: {
					x: 0,
					y: 0
				}
			};
		},
		updated() {
			const rect = this.$el.getBoundingClientRect();
			this.imageEdit.containerWidth = rect.width;
			this.imageEdit.containerHeight = rect.height;
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
						this.current.x = x;
						this.current.y = y;
					},
					onstopdrag: () => {
						this.imageEdit.x += this.current.x;
						this.imageEdit.y += this.current.y;
						this.current.x = 0;
						this.current.y = 0;
						this.updateValue();
					}
				};
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
			isAvailable() {
				return typeof this.value === "string" || "path" in this.value;
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
			path() {
				if (typeof this.value === "string") {
					return this.value;
				}
				return this.value.path;
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
				const path = this.isAvailable ? this.path : this.value.item.fileName;
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
				handler: function() {
					if (this.isImage) {
						this.imagePreload(this.imageUrl);
					}
				}
			},
			value: {
				immediate: true,
				handler: async function() {
					this.imageUrl = null;
					if (this.isAvailable) {
						if (typeof this.config.imageToUrl === "function") {
							this.imageUrl = await this.config.imageToUrl(this.path);
						}
						else if (typeof this.config.imageToUrl === "string") {
							this.imageUrl = this.config.imageToUrl;
						}
					}
				}
			}
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
			imagePreload(url) {
				if (this.imageSrc == url) {
					return;
				}
				let imagePreload = new Image();
				this.isLoading = true;
				imagePreload.onload = () => {
					this.imageSrc = url;
					this.isLoading = false;

					const ratioX = this.imageEdit.containerWidth / imagePreload.width;
					const ratioY = this.imageEdit.containerHeight / imagePreload.height;
					let scale = 1;
					switch (this.imageFill) {
					case "cover":
						scale = ratioX > ratioY ? ratioX : ratioY;
						break;
					case "contain":
						scale = ratioX > ratioY ? ratioY : ratioX;
						break;
					}
					this.imageEdit.width = this.value.width || imagePreload.width * scale;
					this.imageEdit.height = this.value.height || imagePreload.height * scale;
					this.imageEdit.x = (this.imageEdit.containerWidth - this.imageEdit.width) / 2;
					this.imageEdit.y = (this.imageEdit.containerHeight - this.imageEdit.height) / 2;
					this.updateValue();
				};
				imagePreload.onerror = () => {
					this.error = "Cannot load image";
					this.isLoading = false;
				};
				imagePreload.src = url;
			},
			updateValue() {
				const value = Object.assign(
					{
						path: this.path
					},
					this.imageEdit
				);
				this.$emit("input", value);
			}
		}
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
