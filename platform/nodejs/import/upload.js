export default class Upload {

	constructor(config) {
		this.config = Object.assign({
			/**
			 * Upload URL, where to send the file to be uploaded
			 */
			url: null,

			/**
			 * Allow multiple files
			 */
			multi: false,

			/**
			 * Read and parse the upload response. Return the url of the file associated.
			 */
			responseToUrl: (response) => response,

			/**
			 * To be called to monitor the upload progress
			 */
			onProgress: (id, currentBytes, totalBytes) => {},

			/**
			 * To be called upon error
			 */
			onError: (id, message) => {},

			/**
			 * To be called once the upload successfully completes
			 */
			onComplete: (id, url) => {}
		}, config);
	}

	trigger() {
		// Create the uploader
		let input = document.createElement("input");
		input.setAttribute("type", "file");
		if (this.config.multi) {
			input.setAttribute("multiple", true);
		}
		input.style.display = "none";
		input.addEventListener("change", () => {
			handleUpload(input.files);
		}, false);

		// Open the file explorer to select the file
		input.click();
	}

	handleUpload(files) {

		for (let i = 0; i < files.length; i++) {

			const uploadId = i + "-" + Date.now();

			// Create the file form data
			const data = new FormData();
			data.append("file", files[i]);

			// Upload the file and track its progress
			const xhr = new XMLHttpRequest();
 			xhr.upload.addEventListener("progress", (e) => {
				if (e.lengthComputable) {
					this.config.onProgress(uploadId, e.loaded, e.total);
				}
			}, false);
			xhr.onerror = () => {
				this.config.onError(uploadId, "An error occurred during the transaction");
			};
			xhr.onreadystatechange = () => {
				if (xhr.readyState === 4) {
					if (xhr.status === 200) {
						const url = this.responseToUrl(xhr.response);
						this.config.onComplete(uploadId, url);
					}
					else if (xhr.status === 0) {
						// Do nothing (abort)
					}
					else {
						this.config.onError(uploadId, xhr.response || xhr.statusText);
					}
				}
			}

			xhr.open("post", this.url);
			xhr.send(data);
		}
	}
};
