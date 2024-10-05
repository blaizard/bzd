import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
import Cache from "#bzd/nodejs/core/cache.mjs";
import format from "#bzd/nodejs/core/format.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes");

/// A node is a collection of data.
///
/// Data are denominated by a path, each data can be set at various interval.
/// Data have a validity time that is adjusted automatically.
class Node {
	constructor(storage, cache, uid, handlers) {
		this.storage = storage;
		this.cache = cache;
		this.uid = uid;
		this.handlers = handlers;
	}

	/// Hash a array of strings.
	static keyToInternal(...internalsOrKeys) {
		return internalsOrKeys.join("\x01");
	}

	/// Unhash a array of strings.
	static internalToKey(internal) {
		return internal.split("\x01");
	}

	/// Check if a hash starts with a specific key.
	static internalStartsWithKey(internal, key) {
		const hash = key.join("\x01");
		if (internal.startsWith(hash)) {
			if (internal.length == hash.length) {
				return true;
			}
			if (internal[hash.length] == "\x01") {
				return true;
			}
		}
		return false;
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
					paths[Node.keyToInternal(...rootPath, key, subKey)] = subValue;
				}
			} else {
				paths[Node.keyToInternal(...rootPath, key)] = value;
			}
		}
		return paths;
	}

	/// Get the handlers associated with this internal path.
	*getHandlers(internal) {
		for (const [root, handlers] of Object.entries(this.handlers)) {
			if (Node.internalStartsWithKey(internal, root.split("/").filter(Boolean))) {
				for (const [handler, options] of Object.entries(handlers)) {
					yield [handler, options];
				}
			}
		}
	}

	/// Create groups by handlers.
	///
	/// Each group of fragment created will be removed from the fragments and returned separately with the following format,
	/// example using a.b.c.d = 12;
	/// {
	///    [handler]: {
	///        a.b.c: {
	///           options: [handlerOptions],
	///           data: { d: 12 },
	///        }
	///    }
	/// }
	groupByHandler(fragments, handlers) {
		let groups = Object.fromEntries([...handlers].map((key) => [key, {}]));
		const fragmentsRest = Object.fromEntries(
			Object.entries(fragments).filter(([internal, value]) => {
				for (const [handler, options] of this.getHandlers(internal)) {
					if (handler in groups) {
						const path = Node.internalToKey(internal);
						const name = path.pop();
						const parent = Node.keyToInternal(...path);
						groups[handler][parent] ??= {
							options: options,
							data: {},
						};
						groups[handler][parent]["data"][name] = value;
						return false;
					}
				}
				return true;
			}),
		);
		return [fragmentsRest, groups];
	}

	async insert(fragment, ...paths) {
		const timestamp = Date.now();

		let fragments = this.getAllPathAndValues(fragment, paths);
		const [fragmentsRest, groups] = this.groupByHandler(fragments, ["toString"]);

		/// Process the toString handler.
		const toString = Object.fromEntries(
			Object.entries(groups["toString"]).map(([internal, data]) => {
				return [internal, format(data["options"], data["data"])];
			}),
		);

		fragments = Object.assign(fragmentsRest, toString);

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

export default class Nodes {
	constructor(storage, handlers) {
		this.storage = storage.getAccessor("data");
		this.handlers = handlers;
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
				const paths = Node.internalToKey(internal);
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
