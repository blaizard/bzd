import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("apps", "plugin", "bzd");

/// A node is a collection of data.
///
/// Data are denominated by a path, each data can be set at various interval.
/// Data have a validity time that is adjusted automatically.
class Node {
	constructor() {
		this.data = {};
		this.pending = [];
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

	async insert(category, fragment) {
		const timestamp = Date.now();
		let data = await this.get(category);

		// Identify the path of the fragments and their values.
		for (const [path, value] of Object.entries(this.getAllPathAndValues(fragment))) {
			data[path] = {
				timestamp: timestamp,
				value: value,
			};
		}
	}

	async getCategories() {
		return ["data", "pending"];
	}

	async get(category) {
		switch (category) {
			case "pending":
				return this.pending;
			case "data":
				return this.data;
		}
		Exception.error("Invalid category '{}'", category);
	}
}

export default class Nodes {
	constructor(path) {
		this.nodes = {};
	}

	async *getNodes() {
		for (const uid in this.nodes) {
			yield uid;
		}
	}

	async getOrCreate(uid) {
		this.nodes[uid] ??= new Node();
		return this.nodes[uid];
	}

	async get(uid) {
		return await this.nodes[uid];
	}
}
