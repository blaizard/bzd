import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
import Cache from "#bzd/nodejs/core/cache.mjs";
import KeyMapping from "#bzd/apps/artifacts/plugins/nodes/key_mapping.mjs";
import Handlers from "#bzd/apps/artifacts/plugins/nodes/handlers/handlers.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes");
const Log = LogFactory("apps", "plugin", "nodes");

/// A node is a collection of data.
///
/// Data are denominated by a path, each data can be set at various interval.
/// Data have a validity time that is adjusted automatically.
export class Node {
	constructor(storage, cache, uid, handlers) {
		this.storage = storage;
		this.cache = cache;
		this.uid = uid;
		this.handlers = handlers;
	}

	/// Identify the path of the fragments that are being set.
	///
	/// A path is determined by the full path up to a value or a list.
	///
	/// \return A dictionary of paths and values. Each path is hashed using keyToInternal(...).
	getAllPathAndValues(fragment, rootPath = []) {
		let paths = {};
		for (const [key, value] of Object.entries(fragment)) {
			if (value && value.constructor == Object) {
				for (const [subKey, subValue] of Object.entries(this.getAllPathAndValues(value))) {
					paths[KeyMapping.keyToInternal([...rootPath, key, subKey])] = subValue;
				}
			} else {
				paths[KeyMapping.keyToInternal([...rootPath, key])] = value;
			}
		}
		return paths;
	}

	async insert(fragment, ...paths) {
		const timestamp = Date.now();

		let fragments = this.getAllPathAndValues(fragment, paths);
		fragments = this.handlers.processBeforeInsert(fragments);

		await this.storage.update(
			this.uid,
			async (data) => {
				data["timestamp"] = timestamp;

				// Identify the path of the fragments and their values.
				for (const [internal, value] of Object.entries(fragments)) {
					// Prepend the new value and the timestamp to the values array.
					// And ensure there are maximum 10 elements.
					data["data"][internal] ??= { values: [] };
					const values = data["data"][internal].values;
					values.unshift([timestamp, value]);
					while (values.length > 10) {
						values.pop();
					}
				}

				return data;
			},
			{
				timestamp: 0,
				data: {},
			},
		);

		// Invalidate the cache.
		this.cache.setDirty(this.uid);
	}

	async get(...paths) {
		const data = await this.cache.get(this.uid);
		const reducedData = paths.reduce((r, segment) => {
			return r[segment];
		}, data);
		return {
			timestampServer: Date.now(),
			data: reducedData,
		};
	}
}

export class Nodes {
	constructor(storage, handlers) {
		this.storage = storage.getAccessor("data");
		this.handlers = new Handlers(handlers);
		const cache = new Cache({
			garbageCollector: false,
		});
		cache.register("data", async (uid) => {
			// Convert data into a tree.
			let data = await this.storage.get(uid, {});
			// This ensure that existing schema will be updated.
			data = Object.assign(
				{
					data: {},
					timestamp: 0,
				},
				data,
			);
			let tree = {};
			for (const [internal, value] of Object.entries(data.data)) {
				const paths = KeyMapping.internalToKey(internal);
				const lastSegment = paths.pop();
				const object = paths.reduce((r, segment) => {
					if (!r[segment]) {
						r[segment] = {};
					}
					return r[segment];
				}, tree);
				object[lastSegment] = value.values[0];
			}
			return tree;
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
		return new Node(this.storage, this.cache, uid, this.handlers);
	}
}
