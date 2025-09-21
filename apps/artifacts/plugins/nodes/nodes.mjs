import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Handlers from "#bzd/apps/artifacts/plugins/nodes/handlers/handlers.mjs";
import Data from "#bzd/apps/artifacts/plugins/nodes/data.mjs";
import Utils from "#bzd/apps/artifacts/common/utils.mjs";

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
	/// \param key The key at which the entry shall be inserted.
	/// \param fragment The data to be inserted.
	/// \param timestamp The timestamp to use.
	/// \param isFixedTimestamp Whether the timestamp is fixed and shall not be modified or is based on the server time.
	///
	/// \return A list of records corresponding to this change.
	async insert(key, fragment, timestamp = null, isFixedTimestamp = false) {
		let fragments = Node.getAllPathAndValues(fragment, key);
		fragments = this.handlers.processBeforeInsert(fragments);

		timestamp = await this.data.insert(this.uid, fragments, timestamp, /*updateStatistics*/ true);

		// Generate records.
		return fragments.map(([key, value, _]) => [this.uid, key, value, timestamp, isFixedTimestamp]);
	}

	/// Get the tree with single values.
	///
	/// \param key The key to locate the data to be returned.
	/// \param metadata Whether metadata should be returned or not.
	///        It true, a tuple of (timestamp, value, isValid) is returned.
	///        If false, the raw value is returned.
	/// \param children The level of nested children to be added to the result.
	/// \param count Maximal number of values to be returned per entry.
	/// \param after Only return values after this timestamp.
	/// \param before Only return values before this timestamp.
	/// \param include Include the given path to the result.
	/// \param sampling The sampling method to be used.
	///
	/// \return An optional with a value if success, empty if the key points to an unknown record.
	async get({
		key,
		metadata = false,
		children = 0,
		count = null,
		after = null,
		before = null,
		include = null,
		sampling = null,
	}) {
		return await this.data.get({ uid: this.uid, key, metadata, children, count, after, before, include, sampling });
	}

	/// Get children of a given key.
	///
	/// \param key The key to locate the data to be returned.
	/// \param children The level of nested children to be added to the result.
	/// \param includeInner Whether to include inner nodes or not.
	async getChildren(key, children, includeInner) {
		return await this.data.getChildren(this.uid, key, children, includeInner);
	}
}

export class Nodes {
	constructor(handlers, options) {
		this.handlers = new Handlers(handlers);
		this.data = new Data(options);
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
		for (const [uid, key, value, timestamp, _isFixedTimestamp] of records) {
			await this.data.insert(uid, [[key, value]], timestamp, /*updateStatistics*/ false);
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
		for (const [uid, key, value, timestamp, isFixedTimestamp] of record) {
			const keyCluster = uid + "@" + timestamp + "@" + isFixedTimestamp;
			clusters[keyCluster] ??= {
				uid: uid,
				timestamp: timestamp,
				isFixedTimestamp: isFixedTimestamp,
				data: [],
			};
			clusters[keyCluster].data.push([key, value]);
		}
		let onDiskRecord = [];
		for (const [_, cluster] of Object.entries(clusters)) {
			let valueCluster = {};
			for (const [key, value] of cluster.data) {
				let current = valueCluster;
				for (const part of key) {
					current[part] ??= {};
					current = current[part];
				}
				current["_"] = value;
			}
			onDiskRecord.push([cluster.uid, valueCluster, cluster.timestamp, Boolean(cluster.isFixedTimestamp) ? 1 : 0]);
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
		for (const [uid, valueCluster, timestamp, isFixedTimestamp] of record) {
			for (const [key, value] of traverse(valueCluster)) {
				fromDiskRecord.push([uid, key, value, timestamp, Boolean(isFixedTimestamp)]);
			}
		}
		return fromDiskRecord;
	}

	/// Create a record from a single entry.
	///
	/// \param uid The identifier of the node.
	/// \param key The key where to store the value.
	/// \param value The value to be stored.
	/// \param timestamp The timestamp in Ms of this value.
	/// \param isFixedTimestamp If the timestamp is considered fixed or not.
	///
	/// \return A record containing this information.
	static recordFromSingleEntry(uid, key, value, timestamp, isFixedTimestamp = false) {
		return [[uid, key, value, timestamp, isFixedTimestamp]];
	}
}
