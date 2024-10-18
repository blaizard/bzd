import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
import Cache from "#bzd/nodejs/core/cache.mjs";
import KeyMapping from "#bzd/apps/artifacts/plugins/nodes/key_mapping.mjs";
import Handlers from "#bzd/apps/artifacts/plugins/nodes/handlers/handlers.mjs";
import Optional from "#bzd/nodejs/utils/optional.mjs";
import Data from "#bzd/apps/artifacts/plugins/nodes/data.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes");
const Log = LogFactory("apps", "plugin", "nodes");

/// A node is a collection of data.
///
/// Data are denominated by a path, each data can be set at various interval.
/// Data have a validity time that is adjusted automatically.
export class Node {
	constructor(data, uid, handlers) {
		this.data = data;
		this.uid = uid;
		this.handlers = handlers;
	}

	/// Identify the path of the fragments that are being set.
	///
	/// A path is determined by the full path up to a value or a list.
	///
	/// \return A list of paths and values.
	static getAllPathAndValues(fragment, rootKey = []) {
		let paths = [];
		for (const [key, value] of Object.entries(fragment)) {
			if (value && value.constructor == Object) {
				for (const [subKey, subValue] of Node.getAllPathAndValues(value)) {
					paths.push([[...rootKey, key, ...subKey], subValue]);
				}
			} else {
				paths.push([[...rootKey, key], value]);
			}
		}
		return paths;
	}

	/// Insert new data at a given path.
	async insert(key, fragment) {
		let fragments = Node.getAllPathAndValues(fragment, key);
		fragments = this.handlers.processBeforeInsert(fragments);

		await this.data.insert(this.uid, fragments);
	}

	/// Get the tree with single values.
	///
	/// \param key The key to locate the data to be returned.
	/// \param metadata Whether metadata should be returned or not.
	///        If false, the raw value is returned.
	/// \param children Whether children should be returned as well.
	/// \param count Maximal number of values to be returned per entry.
	/// \param after Only return values after this timestamp.
	/// \param before Only return values before this timestamp.
	///
	/// \return An optional with a value if success, empty if the key points to an unknown record.
	async get(key, metadata = false, children = false, count = null, after = null, before = null) {
		return await this.data.get(this.uid, key, metadata, children, count, after, before);
	}

	/// Get direct children of a given key.
	///
	/// TODO: this should not be needed.
	async getChildren(key) {
		return await this.data.getChildren(this.uid, key);
	}
}

export class Nodes {
	constructor(storage, handlers) {
		this.storage = storage.getAccessor("data");
		this.handlers = new Handlers(handlers);
		this.data = new Data(this.storage);
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
		return new Node(this.data, uid, this.handlers);
	}
}
