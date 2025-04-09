import Path from "path";

import ExceptionFactory from "../../core/exception.mjs";
import LogFactory from "../../core/log.mjs";
import { CollectionPaging } from "../utils.mjs";
import { fromChunk, toBuffer } from "#bzd/nodejs/core/stream.mjs";

import Permissions from "./permissions.mjs";
import { Storage, FileNotFoundError } from "./storage.mjs";

const Log = LogFactory("db", "storage", "memory");
const Exception = ExceptionFactory("db", "storage", "memory");

class File {
	constructor(content, date) {
		this.content = content;
		this.created = date;
		this.modified = date;
	}

	get size() {
		return String(this.content).length;
	}
}

/// In-memory virtual file system.
export default class StorageMemory extends Storage {
	constructor(data, options) {
		super(
			Object.assign(
				{
					/// Use a custom date object.
					date: () => new Date(),
				},
				options,
			),
		);

		const count = { "file(s)": 0, directories: 0 };
		const initializeData = (init) => {
			let data = {};
			for (const [key, value] of Object.entries(init)) {
				if (value !== null && typeof value === "object") {
					data[key] = initializeData(value);
					++count.directories;
				} else {
					data[key] = new File(value, this.options.date());
					++count["file(s)"];
				}
			}
			return data;
		};
		this.data = initializeData(data);

		Log.info(
			"Using memory DB, initialized with {}.",
			Object.entries(count)
				.filter(([_, nb]) => nb > 0)
				.map(([k, v]) => v + " " + k)
				.join(", "),
		);
	}

	async _initialize() {}

	_dataToMetadata(data, name) {
		const isFile = data instanceof File;
		const isDirectory = !isFile && data !== null && typeof data === "object";

		Exception.assertPrecondition(isFile || isDirectory, "Entry '{}' does not exists.", name);
		return Permissions.makeEntry(
			{
				name: name,
				type: isDirectory ? "directory" : Path.extname(name).slice(1),
				size: isDirectory ? undefined : data.size,
				created: isDirectory ? undefined : data.created,
				modified: isDirectory ? undefined : data.modified,
			},
			{
				read: true,
				write: true,
				delete: true,
				list: isDirectory,
			},
		);
	}

	_navigateTo(pathList) {
		let parent = this.data;
		for (const part of pathList) {
			Exception.assertPrecondition(!(parent instanceof File), "Entry '{}' must be a directory.", part);
			if (!(part in parent)) {
				throw new FileNotFoundError(part);
			}
			parent = parent[part];
		}
		return parent;
	}

	_getParentNamePair(pathList) {
		Exception.assertPrecondition(pathList.length > 0, "Cannot get entry information from an empty path.");
		const parent = this._navigateTo(pathList.slice(0, -1));
		Exception.assertPrecondition(!(parent instanceof File), "Entry '{}' is a file.", parent);
		const name = pathList[pathList.length - 1];
		return [parent, name];
	}

	async _isImpl(pathList) {
		if (pathList.length === 0) {
			return true;
		}
		try {
			const [parent, name] = this._getParentNamePair(pathList);
			return name in parent;
		} catch (e) {
			if (e instanceof FileNotFoundError) {
				return false;
			}
			throw e;
		}
	}

	async _listImpl(pathList, maxOrPaging, includeMetadata) {
		const parent = this._navigateTo(pathList);
		if (includeMetadata) {
			return CollectionPaging.makeFromList(Object.keys(parent), maxOrPaging, (name) => {
				return this._dataToMetadata(parent[name], name);
			});
		}
		return CollectionPaging.makeFromList(Object.keys(parent), maxOrPaging);
	}

	async _metadataImpl(pathList) {
		const data = this._navigateTo(pathList);
		return this._dataToMetadata(data, pathList.length ? pathList.at(-1) : "");
	}

	async _readImpl(pathList) {
		const [parent, name] = this._getParentNamePair(pathList);
		Exception.assertPrecondition(parent[name] instanceof File, "'{}' points to a directory.", name);
		return fromChunk(parent[name].content);
	}

	async _writeImpl(pathList, readStream) {
		const [parent, name] = this._getParentNamePair(pathList);
		const content = (await toBuffer(readStream)).toString("utf8");
		if (name in parent) {
			Exception.assertPrecondition(parent[name] instanceof File, "'{}' points to a directory.", name);
			parent[name].content = content;
			parent[name].modified = this.options.date();
		} else {
			parent[name] = new File(content, this.options.date());
		}
	}

	async _mkdirImpl(pathList) {
		let parent = this.data;
		for (const part of pathList) {
			if (!(part in parent)) {
				parent[part] = {};
			}
			parent = parent[part];
			Exception.assertPrecondition(!(parent instanceof File), "Entry '{}' must be a directory.", part);
		}
	}

	async _deleteImpl(pathList) {
		const [parent, name] = this._getParentNamePair(pathList);
		delete parent[name];
	}
}
