import { google } from "googleapis";
import Path from "path";

import ExceptionFactory from "../../core/exception.mjs";
import LogFactory from "../../core/log.mjs";
import { CollectionPaging } from "../utils.mjs";

import Permissions from "./permissions.mjs";
import Storage from "./storage.mjs";
import Cache from "#bzd/nodejs/core/cache.mjs";

const Log = LogFactory("db", "storage", "google-drive");
const Exception = ExceptionFactory("db", "storage", "google-drive");

class Cache2 {
	constructor(options) {
		this.options = Object.assign(
			{
				// Default timeout for the validity of an entry.
				timeoutMs: 30 * 1000,
			},
			options,
		);

		// Contains all the cached entries.
		//
		// Each entry has the following format:
		// - [collection]: {
		//      fetch: <function>,
		//      options: {
		//          ....
		//      },
		//      values: { [key]: {
		//          value: undefined,
		//          error: undefined,
		//          size: undefined,
		//          timeout: undefined
		//      }}
		// }
		this.data = {};
	}

	/// Defines an empty state.
	static empty = Symbol();

	/// Current timestamp in milliseconds.
	static getTimestampMs() {
		return Date.now();
	}

	/// Helper to convert an array of string into a key.
	static arrayOfStringToKey(array) {
		return ["", ...array].join("\0");
	}

	/// Helper to convert a key into an array of string.
	static keyToArrayOfString(payload) {
		const [empty, ...path] = payload.split("\0");
		return path;
	}

	/// Register a new data collection.
	///
	/// \param collection The name of the collection.
	/// \param fetch A function to fetch the new data.
	/// \param options Options specific to this collection.
	register(collection, fetch, options) {
		Exception.assert(!(collection in this.data), "Collection '{}' already registered.", collection);
		this.data[collection] = {
			fetch: fetch,
			options: Object.assign(
				{
					// Default timeout for the validity of an entry.
					timeoutMs: this.options.timeoutMs,
					// Get the size of the value.
					getSize: undefined,
				},
				options,
			),
			// Here a map is used instead of the dictionary for the following reasons:
			// 1. Performance: It performs better in scenarios involving frequent additions and removals of key-value pairs.
			// 2. Key order: The keys in Map are ordered in a straightforward way: A Map object iterates entries, keys, and values in the order of entry insertion.
			values: new Map(),
		};
	}

	/// Manually set a value for the collection.
	///
	/// \param collection The name of the collection.
	/// \param key The key for this value.
	/// \param value The value to be set.
	set(collection, key, value) {
		Exception.assert(collection in this.data, "Collection '{}' doesn't exist.", collection);
		const data = this.data[collection];
		// Touch the value, this is important to set the value on top.
		data.values.delete(key);
		data.values.set(key, {
			value: value,
			size: data.options.getSize ? data.options.getSize(value) : undefined,
			timeout: Cache2.getTimestampMs() + data.options.timeoutMs,
		});
	}

	/// Access a value or fetch a new value.
	///
	/// \param collection The name of the collection.
	/// \param key The key for this value.
	async get(collection, key) {
		// Check if the value is already available.
		const maybeValue = this.getInstant(collection, key, Cache2.empty);
		if (maybeValue !== Cache2.empty) {
			return maybeValue;
		}

		// TODO: handle the case of concurrency, if the value is being fetched
		// and another caller issues a get(...).

		const data = this.data[collection];
		try {
			const value = await data.fetch(key);
			this.set(collection, key, value);
		} catch (e) {
			data.values.set(key, {
				error: e,
				timeout: Cache2.getTimestampMs() + data.options.timeoutMs,
			});
		}
		return data.values.get(key).value;
	}

	/// Get the value instantly if available.
	///
	/// \param collection The name of the collection.
	/// \param key The key for this value.
	/// \param defaultValue The default value if the data is not available.
	getInstant(collection, key, defaultValue) {
		Exception.assert(collection in this.data, "Collection '{}' doesn't exist.", collection);
		const data = this.data[collection];
		if (!data.values.has(key)) {
			return defaultValue;
		}
		const entry = data.values.get(key);
		if (entry.timeout < Cache2.getTimestampMs()) {
			return defaultValue;
		}
		if (entry.error) {
			throw data.values[key].error;
		}

		// Touch the value.
		data.values.delete(key);
		data.values.set(key, entry);

		return entry.value;
	}

	/// Mark the current data as invalidated (out of date) so it will be reloaded at the next access.
	///
	/// \param collection The collection to get data from.
	/// \param key The key of the data.
	setDirty(collection, key) {
		Exception.assert(collection in this.data, "Collection '{}' doesn't exist.", collection);
		const data = this.data[collection];
		if (data.values.has(key)) {
			data.values.get(key).timeout = Cache2.getTimestampMs() - 1;
		}
	}
}

/// Google drive abstraction layer.
export default class StorageGoogleDrive extends Storage {
	constructor(folderId, options) {
		super();
		this.drive = null;
		this.folderId = folderId;
		this.cache = new Cache2();
		this.options = Object.assign(
			{
				/// The path of the keyFile for the service account to access the google drive.
				keyFile: null,
			},
			options,
		);

		this.cache.register(
			"id",
			async (key) => {
				const pathList = Cache2.keyToArrayOfString(key);
				if (pathList.length == 0) {
					return this.folderId;
				}

				const parentId = await this.cache.get("id", Cache2.arrayOfStringToKey(pathList.slice(0, -1)));
				const escapedFolderName = pathList.at(-1).replace(/'/g, "''");
				const query = "'" + parentId + "' in parents and name='" + escapedFolderName + "' and trashed=false";
				const response = await this.drive.files.list({
					q: query,
					supportsAllDrives: true,
					includeItemsFromAllDrives: true,
					fields: "files(id)",
				});
				const files = response.data.files;
				if (files.length !== 1) {
					return null;
				}
				return files[0].id;
			},
			{
				getSize: (id) => String(id).length,
			},
		);

		Log.info("Using Google drive DB with folder ID '{}'.", folderId);
	}

	async _initialize() {
		const auth = new google.auth.GoogleAuth({
			keyFile: this.options.keyFile,
			scopes: ["https://www.googleapis.com/auth/drive.readonly"],
		});

		const client = await auth.getClient();
		this.drive = google.drive({ version: "v3", auth: client });
	}

	async _listImpl(pathList, maxOrPaging, includeMetadata) {
		const id = await this.cache.get("id", Cache2.arrayOfStringToKey(pathList));
		const paging = CollectionPaging.pagingFromParam(maxOrPaging);
		const result = await this.drive.files.list({
			q: "'" + id + "' in parents and trashed=false",
			supportsAllDrives: true,
			includeItemsFromAllDrives: true,
			pageToken: paging.page || undefined,
			pageSize: paging.max,
			fields: "nextPageToken, files(id, name, createdTime, modifiedTime, size, capabilities, mimeType)",
		});

		// Format the result.
		const nextPage = result.data.nextPageToken || null;
		let content = [];
		if (includeMetadata) {
			content = result.data.files.map((entry) => {
				const isDirectory = entry.capabilities.canListChildren;
				const isGoogleDocument = entry.mimeType.startsWith("application/vnd.google-apps");
				const documentType = isGoogleDocument ? entry.mimeType.substring(28) : Path.extname(entry.name).slice(1);
				// Save the ID in the cache so we don't have to fetch it twice.
				this.cache.set("id", [...pathList, entry.name], entry.id);
				return Permissions.makeEntry(
					{
						name: entry.name,
						type: isDirectory ? "directory" : documentType,
						size: "size" in entry ? parseInt(entry.size) : undefined,
						created: entry.createdTime,
						modified: entry.modifiedTime,
					},
					{
						// Google documents cannot be downloaded directly, then need to be exported.
						read: !isGoogleDocument && entry.capabilities.canDownload,
						write: entry.capabilities.canEdit,
						delete: entry.capabilities.canDelete,
						list: isDirectory,
					},
				);
			});
		} else {
			content = result.data.files.map((entry) => entry.name);
		}

		return new CollectionPaging(content, nextPage ? { page: nextPage, max: paging.max } : null);
	}

	async _readImpl(pathList) {
		const id = await this.cache.get("id", Cache2.arrayOfStringToKey(pathList));
		const response = await this.drive.files.get(
			{
				fileId: id,
				// Important: Downloads the file content.
				alt: "media",
			},
			{
				// Get response as a stream.
				responseType: "stream",
			},
		);

		return response.data;
	}
}
