import Path from "path";

import ExceptionFactory from "../../core/exception.mjs";
import LogFactory from "../../core/log.mjs";
import { CollectionPaging } from "../utils.mjs";
import { fromChunk } from "#bzd/nodejs/core/stream.mjs";

import Permissions from "./permissions.mjs";
import Storage from "./storage.mjs";

const Log = LogFactory("db", "storage", "memory");
const Exception = ExceptionFactory("db", "storage", "memory");

class File {
	constructor(content) {
		this.content = content;
		this.created = new Date();
		this.modified = new Date();
	}

	get size() {
		return String(this.content).length;
	}
}

/// In-memory virtual file system.
export default class StorageMemory extends Storage {
	constructor(data, options) {
		super();
		this.options = Object.assign({}, options);

		const initializeData = (init) => {
			let data = {};
			for (const [key, value] of Object.entries(init)) {
				if (value !== null && typeof value === "object") {
					data[key] = initializeData(value);
				} else {
					data[key] = new File(value);
				}
			}
			return data;
		};
		this.data = initializeData(data);

		Log.info("Using memory DB.");
	}

	async _initialize() {}

	_dataToMetadata(parent, name) {
		Exception.assertPrecondition(name in parent, "Entry '{}' does not exists.", name);
		const isDirectory = !(parent[name] instanceof File);
		return Permissions.makeEntry(
			{
				name: name,
				type: isDirectory ? "directory" : Path.extname(name).slice(1),
				size: isDirectory ? undefined : parent[name].size,
				created: isDirectory ? undefined : parent[name].created,
				modified: isDirectory ? undefined : parent[name].modified,
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
			Exception.assertPrecondition(part in parent, "Entry '{}' does not exists.", part);
			Exception.assertPrecondition(!(parent[part] instanceof File), "Entry '{}' is a file.", part);
			parent = parent[part];
		}
		return parent;
	}

	_getParentNamePair(pathList) {
		Exception.assertPrecondition(pathList.length > 0, "Cannot get entry information from an empty path.");
		const parent = this._navigateTo(pathList.slice(0, -1));
		const name = pathList[pathList.length - 1];
		return [parent, name];
	}

	async _isImpl(pathList) {
		const [parent, name] = this._getParentNamePair(pathList);
		return name in parent;
	}

	async _listImpl(pathList, maxOrPaging, includeMetadata) {
		const parent = this._navigateTo(pathList);
		if (includeMetadata) {
			return CollectionPaging.makeFromList(Object.keys(parent), maxOrPaging, (name) => {
				return this._dataToMetadata(parent, name);
			});
		}
		return CollectionPaging.makeFromList(Object.keys(parent), maxOrPaging);
	}

	async _metadataImpl(pathList) {
		const [parent, name] = this._getParentNamePair(pathList);
		return this._dataToMetadata(parent, name);
	}

	async _readImpl(pathList) {
		const [parent, name] = this._getParentNamePair(pathList);
		Exception.assertPrecondition(parent[name] instanceof File, "'{}' points to a directory.", name);
		return fromChunk(parent[name].content);
	}

	async _writeImpl(pathList, readStream) {
		const [parent, name] = this._getParentNamePair(pathList);
		const content = (await streamToBuffer(readStream)).toString("utf8");
		if (name in parent) {
			Exception.assertPrecondition(parent[name] instanceof File, "'{}' points to a directory.", name);
			parent[name].content = content;
			parent[name].modified = new Date();
		} else {
			parent[name] = new File(content);
		}
	}

	async _deleteImpl(pathList) {
		const [parent, name] = this._getParentNamePair(pathList);
		delete parent[name];
	}
}
