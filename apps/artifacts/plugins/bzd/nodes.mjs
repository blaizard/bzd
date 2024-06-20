import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
import Cache from "#bzd/nodejs/core/cache.mjs";

const Exception = ExceptionFactory("apps", "plugin", "bzd");

/// A node is a collection of data.
///
/// Data are denominated by a path, each data can be set at various interval.
/// Data have a validity time that is adjusted automatically.
class Node {
	constructor(storage, cache, uid) {
		this.storage = storage;
		this.cache = cache;
		this.uid = uid;
	}

	/// Identify the path of the fragments that are being set.
	///
	/// A path is determined by the full path up to a value or a list.
	///
	/// \return A dictionary of paths and values. Each path is a string which segments are joint with dots.
	getAllPathAndValues(fragment) {
		let paths = {};
		for (const [key, value] of Object.entries(fragment)) {
			if (value && value.constructor == Object) {
				for (const [subKey, subValue] of Object.entries(this.getAllPathAndValues(value))) {
					paths[key + "." + subKey] = subValue;
				}
			} else {
				paths[key] = value;
			}
		}
		return paths;
	}

	async insert(fragment, ...paths) {
		const timestamp = Date.now();
		const pathToString = paths.join(".");

		await this.storage.update(
			this.uid,
			async (data) => {
				data["timestamp"] = timestamp;
				if (!("data" in data)) {
					data["data"] = {};
				}

				// Identify the path of the fragments and their values.
				for (const [subPath, value] of Object.entries(this.getAllPathAndValues(fragment))) {
					data["data"][pathToString + "." + subPath] = {
						timestamp: timestamp,
						value: value,
					};
				}

				return data;
			},
			{},
		);

		// Invalidate the cache.
		this.cache.setDirty(this.uid);
	}

	async get(...paths) {
		const data = await this.cache.get(this.uid);
		const reducedData = paths.reduce((r, segment) => {
			return r[segment];
		}, data.data);
		return {
			timestampServer: Date.now(),
			timestamp: data.timestamp,
			data: reducedData,
		};
	}
}

export default class Nodes {
	constructor(storage) {
		this.storage = storage.getAccessor("data");
		const cache = new Cache();
		cache.register("data", async (uid) => {
			// Convert data into a tree.
			const data = await this.storage.get(uid);
			let tree = {};
			for (const [path, value] of Object.entries(data.data || {})) {
				const paths = path.split(".");
				const lastSegment = paths.pop();
				const object = paths.reduce((r, segment) => {
					if (!r[segment]) {
						r[segment] = {};
					}
					return r[segment];
				}, tree);
				object[lastSegment] = value.value;
			}
			return {
				timestamp: data.timestamp || 0,
				data: tree,
			};
		});
		this.cache = cache.getAccessor("data");
	}

	async *getNodes() {
		const it = CollectionPaging.makeIterator(async (maxOrPaging) => {
			return await this.storage.list(maxOrPaging);
		}, 50);
		for await (const [name, _] of it) {
			yield name;
		}
	}

	async get(uid) {
		return new Node(this.storage, this.cache, uid);
	}
}
