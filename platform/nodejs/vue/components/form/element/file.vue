<script>
	"use strict";

	import Upload from "../lib/upload.js"
	import Array from "./array.vue"
	import fileItem from "./file-item.vue"

	export default {
		mixins: [Array],
		data: function() {

			const isUpload = (this.getOption("upload", false) !== false);
			let templateAdd = [];
			if (isUpload) {
				templateAdd.push({ type: "Submit", content: {html: "<i class=\"irform-icon-upload\"></i> Upload"}, click: this.triggerUpload });
			}

			return {
				upload: (isUpload) ? new Upload({
					/**
					 * Upload URL, where to send the file to be uploaded
					 */
					url: this.getOption("upload"),
					max: () => (this.nbLeft),
					filter: this.getOption("filter", null),
					onInit: (item) => {
						this.uploadValueList.push(item);
					},
					onCancel: (item) => {
						const index = this.uploadItemToIndex(item);
						this.uploadValueList.splice(index, 1);
					},
					onError: (item, message) => {
						this.setError(message);
						console.error(message);
					},
					onComplete: (item, response) => {
						const output = this.uploadResponseHandler(response);

						// Delete the current element
						const index = this.uploadItemToIndex(item);
						this.uploadValueList.splice(index, 1);

						// Add the new item
						this.itemAdd(output);
					}
				}) : null,
				/**
				 * Read and parse the upload response. Return the url or path of the file associated.
				 */
				uploadResponseHandler: this.getOption("uploadResponseHandler", (response) => response),
				/**
				 * Convert the image path to an accessible image url
				 */
				imageToUrl: this.getOption("imageToUrl", (path) => null),

				// ---- Override data from Array ----
				template: fileItem,
				templateAdd: templateAdd,
				allowDelete: false,
				inline: true,
				gripHandle: false,
				// To track the current uploads
				uploadValueList: []
			}
		},
		mounted() {
			if (this.upload) {
				// Add drag and drop events
				this.$el.addEventListener("dragenter", this.dragEnter, false);
				this.$el.addEventListener("dragover", this.dragOver, false);
				this.$el.addEventListener("drop", this.drop, false);

				// Attach global drag event
				document.addEventListener("dragenter", this.globalStartDrag, false);
				document.addEventListener("dragleave", this.dragLeave, false);
			}
		},
		beforeDestroy() {
			if (this.upload) {
				// Remove global drag event
				document.removeEventListener("dragenter", this.globalStartDrag, false);
				document.removeEventListener("dragleave", this.dragLeave, false);
			}
		},
		computed: {
			// Overload internal attributes
			containerClass() {
				return "irform-array irform-array-file";
			},
			valueListToDisplay() {
				return this.valueList.concat(this.uploadValueList);
			},
			templateComponentConfig() {
				return {
					imageToUrl: this.imageToUrl
				};
			},
			browserClientWidth() {
				return window.innerWidth || document.documentElement.clientWidth || document.body.clientWidth;
			},
			browserClientHeight() {
				return window.innerHeight || document.documentElement.clientHeight || document.body.clientHeight;
			}
		},
		methods: {
			globalStartDrag() {
				// Note from the documentation: "If these classes already exist in the element's class attribute they are ignored."
				document.body.classList.add("irform-file-drag");
			},
			globalStopDrag() {
				// Note from the documentation: "Removing a class that does not exist does NOT throw an error."
				document.body.classList.remove("irform-file-drag");
			},
			dragLeave(e) {
				if (e.clientX <= 0 || e.clientY <= 0 || e.clientX >= this.browserClientWidth || e.clientY >= this.browserClientHeight) {
					this.globalStopDrag();
				}
			},
			dragEnter(e) {
				e.stopPropagation();
				e.preventDefault();
			},
			dragOver(e) {
				e.stopPropagation();
				e.preventDefault();
			},
			drop(e) {
				e.stopPropagation();
				e.preventDefault();
				this.upload.handleFiles(e.dataTransfer.files, /*nothrow*/true);
				this.globalStopDrag();
			},

			uploadItemToIndex(item) {
				return this.uploadValueList.findIndex((uploadItem) => uploadItem.uid === item.uid);
			},

			itemDelete(index) {
				// Means this is about the uploadValueList
				if (index >= this.valueList.length) {
					const indexUpload = index - this.valueList.length;
					this.uploadValueList[indexUpload].cancel();
				}
				else {
					Array.methods.itemDelete.call(this, index);
				}
			},

			async triggerUpload() {
				await this.upload.trigger(/*nothrow*/true);
			}
		}
	}
</script>
