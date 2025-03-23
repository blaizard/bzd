import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Handlers from "#bzd/apps/artifacts/plugins/nodes/handlers/handlers.mjs";
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
		if (fragment && fragment.constructor == Object) {
			let paths = [];
			for (const [key, value] of Object.entries(fragment)) {
				for (const [subKey, subValue] of Node.getAllPathAndValues(value)) {
					paths.push([[...rootKey, key, ...subKey], subValue]);
				}
			}
			return paths;
		}
		return [[[...rootKey], fragment]];
	}

	/// Insert new data at a given path.
	///
	/// \return A list of records corresponding to this change.
	async insert(key, fragment, timestampDelta = 0) {
		let fragments = Node.getAllPathAndValues(fragment, key);
		fragments = this.handlers.processBeforeInsert(fragments);

		const timestamp = await this.data.insert(this.uid, fragments, Data.getTimestamp() + timestampDelta);

		// Generate records.
		return fragments.map(([key, value, _]) => [this.uid, key, value, timestamp]);
	}

	/// Get the tree with single values.
	///
	/// \param key The key to locate the data to be returned.
	/// \param metadata Whether metadata should be returned or not.
	///        If false, the raw value is returned.
	/// \param children The level of nested children to be added to the result.
	/// \param count Maximal number of values to be returned per entry.
	/// \param after Only return values after this timestamp.
	/// \param before Only return values before this timestamp.
	/// \param include Include the given path to the result.
	///
	/// \return An optional with a value if success, empty if the key points to an unknown record.
	async get({ key, metadata = false, children = 0, count = null, after = null, before = null, include = null }) {
		return await this.data.get({ uid: this.uid, key, metadata, children, count, after, before, include });
	}

	/// Get direct children of a given key.
	///
	/// TODO: this should not be needed.
	async getChildren(key) {
		return await this.data.getChildren(this.uid, key);
	}
}

export class Nodes {
	constructor(handlers) {
		this.handlers = new Handlers(handlers);
		this.data = new Data();
	}

	async *getNodes() {
		const all = await this.data.list();
		for await (const name of all) {
			yield name;
		}
	}

	async get(uid) {
		return new Node(this.data, uid, this.handlers);
	}

	// Insert a record entry to the data.
	async insertRecord(records) {
		for (const [uid, key, value, timestamp] of records) {
			await this.data.insert(uid, [[key, value]], timestamp);
		}
	}

	/// Write a record to the disk.
	///
	/// The process is made to reduce disk space.
	///
	/// \param record The original record.
	///
	/// \return The disk optimized record.
	static recordToDisk(record) {
		let clusters = {};
		for (const [uid, key, value, timestamp] of record) {
			const keyCluster = uid + "@" + timestamp;
			clusters[keyCluster] ??= [];
			clusters[keyCluster].push([key, value]);
		}
		let onDiskRecord = [];
		for (const [keyCluster, cluster] of Object.entries(clusters)) {
			const [uid, timestampStr] = keyCluster.split("@");
			let valueCluster = {};
			for (const [key, value] of cluster) {
				let current = valueCluster;
				for (const part of key) {
					current[part] ??= {};
					current = current[part];
				}
				current["_"] = value;
			}
			onDiskRecord.push([uid, valueCluster, parseInt(timestampStr)]);
		}
		return onDiskRecord;
	}

	/// Read a record form the disk.
	///
	/// \param record The disk optimized record.
	///
	/// \return The original record.
	static recordFromDisk(record) {
		let fromDiskRecord = [];
		const traverse = (fragment, key = [], depth = 0) => {
			Exception.assert(depth < 32, "Recursive function depth exceeded: {}", depth);
			let paths = [];
			for (const [part, data] of Object.entries(fragment)) {
				if (part == "_") {
					paths.push([key, data]);
				} else {
					paths = paths.concat(traverse(data, [...key, part], depth + 1));
				}
			}
			return paths;
		};
		for (const [uid, valueCluster, timestamp] of record) {
			for (const [key, value] of traverse(valueCluster)) {
				fromDiskRecord.push([uid, key, value, timestamp]);
			}
		}
		return fromDiskRecord;
	}
}
