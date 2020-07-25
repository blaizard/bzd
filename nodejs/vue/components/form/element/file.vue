<script>
	import Vue from "vue";
	import Upload from "../../../../core/upload.mjs";
	import ArrayElement from "./array.vue";
	import FileItem from "./file_item.vue";

	export default {
		mixins: [ArrayElement],
		data: function () {
			let templateAdd = [];

			// Add the upload button
			const uploadContent = this.getOption("uploadContent", '<i class="bzd-icon-upload"></i> Upload');
			templateAdd.push({
				type: "Button",
				content: { html: uploadContent },
				click: this.triggerUpload,
				align: this.getOption("align", "left"),
			});

			const allowDelete = this.getOption("allowDelete", true);
			const templateClass = this.getOption("templateClass", "");
			const templateEdit = this.getOption("templateEdit", false);
			const imageFill = this.getOption("imageFill", "cover");
			return {
				/**
				 * Read and parse the upload response. Return the url or path of the file associated.
				 */
				uploadResponseHandler: this.getOption("uploadResponseHandler", (response) => response),
				/**
				 * Convert the image path to an accessible image url
				 */
				imageToUrl: this.getOption("imageToUrl", (/*path*/) => null),

				// ---- Override data from ArrayElement ----
				template: Vue.extend({
					mixins: [FileItem],
					data: function () {
						return {
							allowDelete: allowDelete,
							templateClass: templateClass,
							templateEdit: templateEdit,
							imageFill: imageFill,
						};
					},
				}),
				templateAdd: templateAdd,
				allowDelete: false,
				inline: true,
				gripHandle: false,
				// To track the current uploads
				uploadValueList: [],
			};
		},
		mounted() {
			if (this.isUpload) {
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
			if (this.isUpload) {
				// Remove global drag event
				document.removeEventListener("dragenter", this.globalStartDrag, false);
				document.removeEventListener("dragleave", this.dragLeave, false);
			}
		},
		computed: {
			isUpload() {
				return this.getOption("upload", false) !== false;
			},
			upload() {
				return this.isUpload
					? new Upload({
							/**
							 * Upload URL, where to send the file to be uploaded
							 */
							url: this.getOption("upload"),
							max: () => this.nbLeft,
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
							},
					  })
					: null;
			},
			// Overload internal attributes
			containerClass() {
				return {
					"irform-array": true,
					"irform-array-file": true,
					"irform-fill": this.fill,
					[this.getOption("class")]: true,
				};
			},
			valueListToDisplay() {
				return this.valueList.concat(this.uploadValueList);
			},
			templateComponentConfig() {
				return {
					imageToUrl: this.imageToUrl,
				};
			},
			browserClientWidth() {
				return window.innerWidth || document.documentElement.clientWidth || document.body.clientWidth;
			},
			browserClientHeight() {
				return window.innerHeight || document.documentElement.clientHeight || document.body.clientHeight;
			},
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
				if (
					e.clientX <= 0 ||
					e.clientY <= 0 ||
					e.clientX >= this.browserClientWidth ||
					e.clientY >= this.browserClientHeight
				) {
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
			async drop(e) {
				e.stopPropagation();
				e.preventDefault();
				// Special case, only 1 file and no slot left
				if (this.max == 1 && this.nbLeft == 0) {
					await this.itemDelete(0);
				}
				this.upload.handleFiles(e.dataTransfer.files, /*nothrow*/ true);
				this.globalStopDrag();
			},

			uploadItemToIndex(item) {
				return this.uploadValueList.findIndex((uploadItem) => uploadItem.uid === item.uid);
			},

			async itemDelete(index) {
				// Means this is about the uploadValueList
				if (index >= this.valueList.length) {
					const indexUpload = index - this.valueList.length;
					this.uploadValueList[indexUpload].cancel();
				} else {
					await ArrayElement.methods.itemDelete.call(this, index);
				}
			},

			async triggerUpload() {
				await this.upload.trigger(/*nothrow*/ true);
			},

			async itemClick(index) {
				await this.upload.trigger(/*nothrow*/ true, {
					beforeUpload: async () => {
						await this.itemDelete(index);
					},
				});
			},
		},
	};
</script>

<style lang="scss">
	@use "bzd-style/css/form/file.scss";
</style>
