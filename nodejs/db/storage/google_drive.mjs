import { google } from "googleapis";
import Path from "path";

import ExceptionFactory from "../../core/exception.mjs";
import LogFactory from "../../core/log.mjs";
import { CollectionPaging } from "../utils.mjs";

import Permissions from "./permissions.mjs";
import Storage from "./storage.mjs";
import Cache2 from "#bzd/nodejs/core/cache2.mjs";

const Log = LogFactory("db", "storage", "google-drive");
const Exception = ExceptionFactory("db", "storage", "google-drive");

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

	_fileDataTometadata(data) {
		const isDirectory = data.capabilities.canListChildren;
		const isGoogleDocument = data.mimeType.startsWith("application/vnd.google-apps");
		const documentType = isGoogleDocument ? data.mimeType.substring(28) : Path.extname(data.name).slice(1);

		return Permissions.makeEntry(
			{
				name: data.name,
				type: isDirectory ? "directory" : documentType,
				size: "size" in data ? parseInt(data.size) : undefined,
				created: data.createdTime,
				modified: data.modifiedTime,
			},
			{
				// Google documents cannot be downloaded directly, then need to be exported.
				read: !isGoogleDocument && data.capabilities.canDownload,
				write: data.capabilities.canEdit,
				delete: data.capabilities.canDelete,
				list: isDirectory,
			},
		);
	}

	async _metadataImpl(pathList) {
		const id = await this.cache.get("id", Cache2.arrayOfStringToKey(pathList));
		const response = await this.drive.files.get({
			fileId: id,
			supportsAllDrives: true,
			includeItemsFromAllDrives: true,
			fields: "id, name, createdTime, modifiedTime, size, capabilities, mimeType",
		});
		return this._fileDataTometadata(response.data);
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
				// Save the ID in the cache so we don't have to fetch it twice.
				this.cache.set("id", Cache2.arrayOfStringToKey([...pathList, entry.name]), entry.id);
				return this._fileDataTometadata(entry);
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
				supportsAllDrives: true,
				includeItemsFromAllDrives: true,
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

	async _createFile(pathList, readStream) {
		const parentId = await this.cache.get("id", Cache2.arrayOfStringToKey(pathList.slice(0, -1)));
		const fileName = pathList[pathList.length - 1];

		const response = await this.drive.files.create({
			resource: {
				name: fileName,
				parents: parentId,
			},
			media: {
				mimeType: "application/octet-stream",
				body: readStream,
			},
			fields: "id",
		});
		// Save the ID in the cache so we don't have to fetch it twice.
		this.cache.set("id", Cache2.arrayOfStringToKey(pathList), response.data.id);
	}

	async _writeImpl(pathList, readStream) {
		try {
			const id = await this.cache.get("id", Cache2.arrayOfStringToKey(pathList));
			await this.drive.files.update({
				fileId: id,
				media: {
					mimeType: "application/octet-stream",
					body: readStream,
				},
			});
		} catch (error) {
			if (error.errors && error.errors[0].reason === "fileNotFound") {
				await this._createFile(pathList, readStream);
				return;
			}
			throw error;
		}
	}

	async _deleteImpl(pathList) {
		const id = await this.cache.get("id", Cache2.arrayOfStringToKey(pathList));
		await this.drive.files.delete({
			fileId: id,
		});
	}
}
