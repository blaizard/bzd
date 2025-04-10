import Fs from "fs";

import ExceptionFactory from "../../core/exception.mjs";
import { fromChunk, toBuffer } from "../../core/stream.mjs";
import { AsyncInitialize } from "../utils.mjs";

const Exception = ExceptionFactory("db", "storage");

export class FileNotFoundError extends Error {}

/// File storage module.
/// Path are addressed by a path list, as some implementation allow slashes as a name.
export class Storage extends AsyncInitialize {
	constructor(options = {}) {
		super();
		this.options = Object.assign(
			{
				/// Enable write access.
				write: false,
			},
			options,
		);
	}

	/// Check if the storage has write access.
	get writeAccess() {
		return Boolean(this.options.write);
	}

	/// Tell whether a key exists or not
	async is(path) {
		return this._isImpl(Array.isArray(path) ? path : [path]);
	}

	/// List all files under this path
	async list(path, maxOrPaging = 20, includeMetadata = false) {
		return this._listImpl(Array.isArray(path) ? path : [path], maxOrPaging, includeMetadata);
	}

	/// Return a file read stream from a specific key
	async read(path) {
		return this._readImpl(Array.isArray(path) ? path : [path]);
	}

	/// Return the content of a file as a buffer.
	async readToBuffer(path) {
		const readStream = await this.read(path);
		return await toBuffer(readStream);
	}

	/// Store a file to a specific path
	async writeFromFile(path, inputFilePath, mkdir = false) {
		Exception.assert(typeof inputFilePath == "string", "Path must be a string, {:j}", inputFilePath);
		const readStream = Fs.createReadStream(inputFilePath);
		return this.write(path, readStream, mkdir);
	}

	/// Store a file to a specific path
	async writeFromChunk(path, data, mkdir = false) {
		const readStream = fromChunk(data);
		return this.write(path, readStream, mkdir);
	}

	/// Store a file to a specific path
	async write(path, readStream, mkdir = false) {
		Exception.assert(this.writeAccess === true, "This storage doesn't have write access.");
		const pathList = Array.isArray(path) ? path : [path];
		if (mkdir) {
			await this.mkdir(pathList.slice(0, -1));
		}
		return this._writeImpl(pathList, readStream);
	}

	/// Delete a file or directory from a path
	///
	/// \throws FileNotFoundError if the file or directory does not exists.
	async delete(path) {
		Exception.assert(this.writeAccess === true, "This storage doesn't have write access.");
		return this._deleteImpl(Array.isArray(path) ? path : [path]);
	}

	/// Create a directory at a given path.
	///
	/// If the parent directory also does not exists, create it as well.
	///
	/// \throws only if the path already exists AND is a file.
	async mkdir(path) {
		Exception.assert(this.writeAccess === true, "This storage doesn't have write access.");
		return this._mkdirImpl(Array.isArray(path) ? path : [path]);
	}

	/// Get metadata of a specific file
	///
	/// Metadata have special fields:
	/// - name: The name of the file.
	/// - size: The size in bytes of the file (number).
	/// - created: The creation date (Date type).
	/// - modified: The modification date (Date type).
	/// - permissions: The permissions of the file.
	async metadata(path) {
		const pathList = Array.isArray(path) ? path : [path];
		if (typeof this._metadataImpl == "function") {
			return this._metadataImpl(pathList);
		}
		return {
			name: pathList[pathList.length - 1],
		};
	}

	/// Set the permissions of a file
	async setPermission(path, permissions) {
		Exception.assert(this.writeAccess === true, "This storage doesn't have write access.");
		return this._setPermissionImpl(Array.isArray(path) ? path : [path], permissions);
	}

	/// Run a set of validation tests
	async runValidationTests(root = "validation_tests") {
		try {
			await this.mkdir([root]);

			// Write and read a file.
			const date = new Date().toUTCString();
			await this.writeFromChunk([root, "a.txt"], "content for a.txt: " + date);
			const content1 = await this.readToBuffer([root, "a.txt"]);
			Exception.assertEqual(
				content1.toString(),
				"content for a.txt: " + date,
				"Issue while writing or reading a file.",
			);

			// Overwrite and read a file.
			await this.writeFromChunk([root, "a.txt"], "a new content for a.txt: " + date);
			const content2 = await this.readToBuffer([root, "a.txt"]);
			Exception.assertEqual(content2.toString(), "a new content for a.txt: " + date, "Issue while overwriting a file.");

			// Create additional files/directories and list them.
			await this.writeFromChunk([root, "b.txt"], "content for b.txt: " + date);
			await this.writeFromChunk([root, "directory", "c.txt"], "content for c.txt: " + date, /*mkdir*/ true);
			const result1 = await this.list([root]);
			Exception.assertEqual(result1.data().sort(), ["a.txt", "b.txt", "directory"], "Issue while listing {}.", root);

			// List nested.
			const result2 = await this.list([root, "directory"]);
			Exception.assertEqual(result2.data().sort(), ["c.txt"], "Issue while listing {}/directory.", root);
		} finally {
			await this.delete(root);
		}
	}
}
