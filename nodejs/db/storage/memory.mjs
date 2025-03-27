import Path from "path";

import ExceptionFactory from "../../core/exception.mjs";
import LogFactory from "../../core/log.mjs";
import { CollectionPaging } from "../utils.mjs";
import { fromChunk } from "#bzd/nodejs/core/stream.mjs";

import Permissions from "./permissions.mjs";
import Storage from "./storage.mjs";

const Log = LogFactory("db", "storage", "memory");
const Exception = ExceptionFactory("db", "storage", "memory");

/// In-memory virtual file system.
export default class StorageMemory extends Storage {
	constructor(data, options) {
		super();
		this.options = Object.assign({}, options);
		this.data = data;

		Log.info("Using memory DB.");
	}

	async _initialize() {}

	_dataToMetadata(parent, name) {
		Exception.assertPrecondition(name in parent, "Entry '{}' does not exists.", name);
		const isDirectory = parent[name] !== null && typeof parent[name] === "object";
		const fileType = Path.extname(name).slice(1);
		return Permissions.makeEntry(
			{
				name: name,
				type: isDirectory ? "directory" : fileType,
				size: isDirectory ? undefined : String(parent[name]).length,
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
			Exception.assertPrecondition(
				parent[part] !== null && typeof parent[part] === "object",
				"Entry '{}' is not a file.",
				part,
			);
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
		Exception.assertPrecondition(
			typeof parent[name] === "string",
			"The file '{}' does not contain data as string.",
			name,
		);
		return fromChunk(parent[name]);
	}

	async _writeImpl(pathList, readStream) {
		const [parent, name] = this._getParentNamePair(pathList);
		parent[name] = (await streamToBuffer(readStream)).toString("utf8");
	}

	async _deleteImpl(pathList) {
		const [parent, name] = this._getParentNamePair(pathList);
		delete parent[name];
	}
}
