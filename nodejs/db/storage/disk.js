import Path from "path";

import ExceptionFactory from "../../core/exception.js";
import FileSystem from "../../core/filesystem.js";
import LogFactory from "../../core/log.js";
import { copy as copyStream } from "../../core/stream.js";
import pathlib from "../../utils/pathlib.js";
import { CollectionPaging } from "../utils.js";

import Permissions from "./permissions.js";
import { Storage, FileNotFoundError } from "./storage.js";

const Log = LogFactory("db", "storage", "disk");
const Exception = ExceptionFactory("db", "storage", "disk");

/// Get the metadata of a file
async function getMetadata(fileSystem, filePath) {
	let isRead = true;
	let isWrite = true;
	let isDelete = true;
	let isList = true;
	let type = "";

	if (!(await fileSystem.exists(filePath))) {
		throw new FileNotFoundError(filePath);
	}

	const stat = await fileSystem.stat(filePath);

	if (stat.isFile()) {
		isList = false;
		type = Path.extname(filePath).slice(1);
	} else if (stat.isDirectory()) {
		isRead = false;
		isWrite = false;
		isDelete = false;
		type = "directory";
	} else {
		isRead = false;
		isWrite = false;
		isDelete = false;
		isList = false;
		if (stat.isBlockDevice() || stat.isCharacterDevice()) {
			type = "device";
		} else if (stat.isFIFO()) {
			type = "pipe";
		} else if (stat.isSocket()) {
			type = "socket";
		}
	}

	return Permissions.makeEntry(
		{
			name: Path.basename(filePath),
			type: type,
			size: stat.size,
			created: stat.ctime,
			modified: stat.mtime,
		},
		{
			read: isRead,
			write: isWrite,
			delete: isDelete,
			list: isList,
		},
	);
}

/// File storage module
export default class StorageDisk extends Storage {
	constructor(path, options) {
		super(
			Object.assign(
				{
					/// If false, it will attempt to create a directory if it does not exist.
					mustExists: false,
					/// File system implementation to use (a mock for tests).
					fs: FileSystem,
				},
				options,
			),
		);
		this.path = path;

		Log.info("Using disk storage DB at '{}'.", this.path);
	}

	/// Initialize the storage module
	async _initialize() {
		if (!(await this.options.fs.exists(this.path))) {
			Exception.assert(!this.options.mustExists, "'{}' is not a valid path.", this.path);
			await this.options.fs.mkdir(this.path);
		}
		this.root = await this.options.fs.realpath(this.path);
	}

	/// Get the full path of the given path, asserting it stays within the volume root.
	async _getFullPath(pathList) {
		const path = pathlib.path(pathList).normalize;
		const fullPath = Path.join(this.path, ...path.parts);
		const resolved = await this._resolveRealPath(fullPath);
		Exception.assertPrecondition(
			resolved === this.root || resolved.startsWith(this.root + Path.sep),
			"The path '{}' is outside of the storage root '{}'.",
			resolved,
			this.root,
		);
		return fullPath;
	}

	/// Resolve the real path of the given path, walking up to the deepest existing ancestor if the leaf does not exist.
	async _resolveRealPath(path) {
		try {
			return await this.options.fs.realpath(path);
		} catch (e) {
			if (e.code !== "ENOENT") {
				throw e;
			}
			const parent = Path.dirname(path);
			if (parent === path) {
				throw e;
			}
			return Path.join(await this._resolveRealPath(parent), Path.basename(path));
		}
	}

	async _isImpl(pathList) {
		return await this.options.fs.exists(await this._getFullPath(pathList));
	}

	async _readImpl(pathList) {
		const path = await this._getFullPath(pathList);
		if (!(await this.options.fs.exists(path))) {
			throw new FileNotFoundError(path);
		}
		return this.options.fs.createReadStream(path);
	}

	async _writeImpl(pathList, readStream) {
		const fullPath = await this._getFullPath(pathList);
		let writeStream = this.options.fs.createWriteStream(fullPath);

		return copyStream(writeStream, readStream);
	}

	async _deleteImpl(pathList) {
		const path = await this._getFullPath(pathList);
		if (!(await this.options.fs.exists(path))) {
			throw new FileNotFoundError(path);
		}
		if ((await this.options.fs.stat(path)).isDirectory()) {
			await this.options.fs.rmdir(path, { force: false });
		} else {
			await this.options.fs.unlink(path);
		}
	}

	async _listImpl(pathList, maxOrPaging, includeMetadata) {
		const fullPath = await this._getFullPath(pathList);
		if (await this.options.fs.exists(fullPath)) {
			const data = await this.options.fs.readdir(fullPath, /*withFileTypes*/ includeMetadata);
			if (includeMetadata) {
				return await CollectionPaging.makeFromList(data, maxOrPaging, async (dirent) => {
					let filePath = fullPath + "/" + dirent.name;
					if (dirent.isSymbolicLink()) {
						// Try to resolve the actual path.
						try {
							filePath = await this.options.fs.realpath(filePath);
						} catch (e) {
							// Ignore
						}
					}

					let metadata = await getMetadata(this.options.fs, filePath);
					// This might differ in case of symlinks
					metadata.name = dirent.name;
					return metadata;
				});
			}
			return await CollectionPaging.makeFromList(data, maxOrPaging);
		}
		throw new FileNotFoundError(fullPath);
	}

	async _mkdirImpl(pathList) {
		const fullPath = await this._getFullPath(pathList);
		await this.options.fs.mkdir(fullPath, { force: true });
	}

	async _metadataImpl(pathList) {
		const filePath = await this._getFullPath(pathList);
		if (!(await this.options.fs.exists(filePath))) {
			throw new FileNotFoundError(filePath);
		}
		return await getMetadata(this.options.fs, filePath);
	}

	async _setPermissionImpl(pathList, permissions) {
		const filePath = await this._getFullPath(pathList);
		if (!(await this.options.fs.exists(filePath))) {
			throw new FileNotFoundError(filePath);
		}

		// Create the permission.
		let mode = 0;
		if (permissions.isRead()) {
			mode |= 0o400 | 0o40 | 0o4;
		}
		if (permissions.isWrite()) {
			mode |= 0o200 | 0o20 | 0o2;
		}
		if (permissions.isExecutable()) {
			mode |= 0o100 | 0o10 | 0o1;
		}
		return await this.options.fs.chmod(filePath, mode);
	}
}
