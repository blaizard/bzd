import Path from "path";

import Storage from "./storage.mjs";
import LogFactory from "../../core/log.mjs";
import ExceptionFactory from "../../core/exception.mjs";
import { CollectionPaging } from "../utils.mjs";
import Permissions from "./permissions.mjs";
import { HttpClientFactory, HttpClientException } from "../../core/http/client.mjs";

const Log = LogFactory("db", "storage", "webdav");
const Exception = ExceptionFactory("db", "storage", "webdav");

// Webdav cloud storage module
export default class StorageWebdav extends Storage {
	constructor(url, options) {
		super();

		this.options = Object.assign(
			{
				username: "",
				password: "",
			},
			options
		);
		this.fetch = new HttpClientFactory(url, {
			authentication: {
				type: "basic",
				username: this.options.username,
				password: this.options.password,
			},
		});

		Log.info("Using WebDAV storage DB at '{}'.", url);
	}

	/// Initialize the storage module.
	async _initialize() {}

	_getLocalPath(pathList) {
		return "/" + pathList.map(encodeURIComponent).join("/");
	}

	_matchTag(str, tag) {
		const regexpr = new RegExp("<(?:d:)?" + tag + ">(.*?)</(?:d:)?" + tag + ">", "ig");
		return [...str.matchAll(regexpr)].map((m) => m[1]);
	}

	_strToJson(str, tags) {
		let output = {};
		for (const tag of tags) {
			const result = this._matchTag(str, tag);
			if (result.length > 0) {
				Exception.assert(result.length == 1, "There must be at most one tag '{}', instead {:j}", tag, result);
				output[tag] = decodeURIComponent(result[0]);
			}
		}
		return output;
	}

	async _isImpl(pathList) {
		const fullPath = this._getLocalPath(pathList);
		try {
			await this.fetch.request(fullPath, {
				method: "propfind",
			});
		}
		catch (e) {
			if (e instanceof HttpClientException) {
				if (e.code == 404 /*Not Found*/) {
					return false;
				}
			}
			throw e;
		}
		return true;
	}

	async _metadataImpl(pathList) {
		const fullPath = this._getLocalPath(pathList);
		const data = await this._request(fullPath);
		return data[0];
	}

	async _readImpl(pathList) {
		const fullPath = this._getLocalPath(pathList);
		const result = await this.fetch.request(fullPath, {
			method: "get",
			expect: "stream",
		});
		return result;
	}

	async _listImpl(pathList, maxOrPaging /*, includeMetadata*/) {
		const fullPath = this._getLocalPath(pathList);
		const data = await this._request(fullPath);
		return await CollectionPaging.makeFromList(data.slice(1), maxOrPaging);
	}

	async _request(fullPath) {
		const result = await this.fetch.request(fullPath, {
			method: "propfind",
		});

		let data = [];
		for (const entry of this._matchTag(result, "response")) {
			const result = this._strToJson(entry, [
				"href",
				"creationdate",
				"getlastmodified",
				"resourcetype",
				"getcontentlength",
				"getetag",
			]);
			const name = Path.basename(result["href"] || "");
			if (name) {
				const isDirectory = "resourcetype" in result;
				data.push(
					Permissions.makeEntry(
						{
							name: name,
							type: isDirectory ? "directory" : Path.extname(name).slice(1),
							modified: "getlastmodified" in result ? new Date(Date.parse(result["getlastmodified"])) : undefined,
							created: "creationdate" in result ? new Date(Date.parse(result["creationdate"])) : undefined,
							size: "getcontentlength" in result ? parseInt(result["getcontentlength"]) : undefined,
						},
						{ read: true, list: isDirectory }
					)
				);
			}
		}

		return data;
	}
}
