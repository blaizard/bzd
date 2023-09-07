import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("apps", "plugin", "bzd");

class Node {
	constructor() {
		this.data = {};
		this.pending = [];
		this.timestamp = null;
	}

	async insert(category, fragment) {
		const timestamp = Date.now();
		let data = await this.get(category);
		Object.assign(data, fragment);
		this.timestamp = timestamp;
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
