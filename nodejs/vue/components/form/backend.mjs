import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import FileSystem from "#bzd/nodejs/core/filesystem.mjs";
import pathlib from "#bzd/nodejs/utils/pathlib.mjs";

const Exception = ExceptionFactory("form");
const Log = LogFactory("form");

/// This class is used to install the REST API for the Form component.
export default class Form {
	/// Constructor for the Form class.
	///
	/// \param uploadDirectory Where all uploaded files will be stored.
	constructor(options = {}) {
		// Here a map is used instead of the dictionary for the following reasons:
		// 1. Key order: The keys in Map are ordered in a straightforward way: A Map object iterates entries, keys, and values in the order of entry insertion.
		this.uploadedFiles = new Map();
		this.uidCounter = 0;
		this.options = Object.assign(
			{
				// Where all uploaded files will be stored.
				uploadDirectory: null,
			},
			options,
		);

		//  Update options.
		this.options.uploadDirectory = this.options.uploadDirectory
			? pathlib.path(this.options.uploadDirectory).absolute()
			: pathlib.tmp().joinPath("form-upload");

		Log.info("Using '{}' for upload.", this.options.uploadDirectory.asPosix());
		FileSystem.mkdirSync(this.options.uploadDirectory.asPosix());
	}

	getUid() {
		return ++this.uidCounter;
	}

	/// Get the uploaded file given its name.
	getUploadFile(name) {
		Exception.assertPrecondition(name in this.uploadedFiles, "File '{}' not found in uploaded files.", name);
		return this.uploadedFiles[name];
	}

	installRest(api) {
		Log.info("Installing 'Form' REST.");

		api.handle("post", "/form/upload", async (inputs) => {
			Exception.assert(inputs.files.length == 1, "There must be exactly 1 file uploaded: {:j}", inputs);
			const originalPath = inputs.files[0];
			Exception.assert(originalPath, "File to be uploaded, evaluates to null: {:j}", originalPath);

			// Move the file to the upload directory.
			const name = "file-" + this.getUid() + "." + pathlib.path(originalPath).suffix;
			const path = this.options.uploadDirectory.joinPath(name).asPosix();
			await FileSystem.move(originalPath, path);
			this.uploadedFiles.set(name, {
				path: path,
			});

			return { path: name };
		});
	}
}
