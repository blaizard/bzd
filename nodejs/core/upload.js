let UIDCounter = 0;

class UploadItem {
	/**
	 * Upload Item
	 */
	constructor(file, xhr) {
		this.file = file;
		this.xhr = xhr;
		this.currentBytes = 0;
		this.error = null;
		this.uid  = Date.now() + "-" + (++UIDCounter) + "-" + this.fileName;
	}

	get totalBytes() {
		return this.file.size;
	}

	get fileName() {
		return this.file.name;
	}

	/**
	 * Cancel the upload
	 */
	cancel() {
		this.xhr.abort();
	}
};

export default class Upload {

	constructor(config) {
		this.config = Object.assign({
			/**
			 * Upload URL, where to send the file to be uploaded
			 */
			url: null,
			/**
			 * Max number of files to be uploaded or a function (if dynamic that returns the max number of files)
			 */
			max: Number.MAX_SAFE_INTEGER,
			/**
			 * Filter the selection with the following categories. Accepted filters are:
			 * - image
			 * - audio
			 * - video
			 * It can be empty, a string or an array of strings
			 */
			filter: null,
			/**
			 * To be called to monitor the upload progress
			 */
			onInit: (item) => {},

			/**
			 * To be called if the upload was manually canceled
			 */
			onCancel: (item) => {},

			/**
			 * To be called to monitor the upload progress
			 */
			onProgress: (item) => {},

			/**
			 * To be called upon error
			 */
			onError: (item, message) => {},

			/**
			 * To be called once the upload successfully completes
			 */
			onComplete: (item, response) => {}
		}, config);
	}

	/**
	 * Manual trigger the upload action
	 *
	 * \param nothrow Set to true if the function should no throw exception
	 */
	async trigger(nothrow) {
		return new Promise((resolve, reject) => {

			// Create the uploader
			let input = document.createElement("input");

			input.setAttribute("type", "file");
			if (((typeof this.config.max === "function") ? this.config.max() : this.config.max) > 1) {
				input.setAttribute("multiple", true);
			}
			// Set the filters
			if (this.config.filter) {
				const filterList = (this.config.filter instanceof Array) ? this.config.filter : [this.config.filter];
				input.setAttribute("accept", filterList.map((filter) => {
					switch (filter) {
					case "image":
						return "image/*";
					case "audio":
						return "audio/*";
					case "video":
						return "video/*";
					}
					return null;
				}).filter((filter) => Boolean(filter)).join(","));
			}
			input.style.display = "none";
			input.addEventListener("change", () => {
				this.handleFiles(input.files, nothrow).then(resolve).catch(reject);
			}, false);

			// Open the file explorer to select the file
			input.click();

			// To be called once only
			const resolveCancel = () => {
				// Should be triggered with a short delay to let onchange event be triggered if necessary
				setTimeout(() => {
					if (!input.files.length) {
						resolve();
					}
					window.removeEventListener("focus", resolveCancel, { once: true });
					document.removeEventListener("mousemove", resolveCancel, { once: true });
				}, 100);
			};

			// Set events to detect the end of 
			window.addEventListener("focus", resolveCancel, { once: true });
			setTimeout(() => {
				document.addEventListener("mousemove", resolveCancel, { once: true });
			}, 1000);
		});
	}

	/**
	 * Handles the files object to be uploaded to the server
	 *
	 * \param files The file list object
	 * \param nothrow Set to true if the function should no throw exception
	 */
	async handleFiles(files, nothrow) {

		const maxNbFiles = Math.min(files.length, ((typeof this.config.max === "function") ? this.config.max() : this.config.max)); 
		let promiseList = [];

		for (let i = 0; i < maxNbFiles; i++) {
			promiseList.push(this.handleFile(files[i], nothrow));
		}

		// Wait for all promises to be completed or throw depending on the configuration
		await Promise.all(promiseList);
	}

	/**
	 * Handle a single file
	 *
	 * \param file The file object
	 * \param nothrow Set to true if the function should no throw exception
	 */
	async handleFile(file, nothrow) {
		return new Promise((resolve, reject) => {

			try {

				// Upload the file and track its progress
				const xhr = new XMLHttpRequest();
				const item = new UploadItem(file, xhr);

				// Helper to handle an error
				const handleError = () => {
					this.config.onError(item, item.error);
					if (nothrow) {
						resolve();
					}
					else {
						reject(new Error(item.error));
					}
				};

		 		xhr.upload.addEventListener("progress", (e) => {
					if (e.lengthComputable) {
						item.currentBytes = e.loaded;
						this.config.onProgress(item);
					}
				}, false);
				xhr.onerror = () => {
					item.error = "An error occurred during the transaction";
					handleError();
				};
				xhr.onabort = () => {
					this.config.onCancel(item);
					resolve();
				};
				xhr.onreadystatechange = () => {
					if (xhr.readyState === 4) {
						if (xhr.status === 200) {
							this.config.onComplete(item, xhr.response);
							resolve();
						}
						else if (xhr.status === 0) {
							// Ignore
						}
						else {
							item.error = (xhr.statusText + " (" + xhr.status + ")") + ": " + xhr.response;
							handleError();
						}
					}
				}

				// Before starting, call the init
				this.config.onInit(item);

				xhr.open("post", this.config.url);

				// Create the file form data
				const data = new FormData();
				data.append("file", file);
				xhr.send(data);

			}
			catch (e) {
				if (!nothrow) {
					reject(e);
				}
				// Else ignore the error
			}
		});
	}
};
