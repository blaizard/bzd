class Node {
	constructor() {
		this.data = {
			status: {},
			records: {},
		};
		this.pending = [];
		this.timestamp = null;
	}

	async insert(fragment, timestamp) {
		Object.assign(this.data, fragment);
		this.timestamp = timestamp;
	}

	async getCategories() {
		return Object.keys(this.data).concat(["pending"]);
	}

	async get(category) {
		if (category == "pending") {
			return this.pending;
		}
		return this.data[category];
	}
}

export default class Nodes {
	constructor(path) {
		this.nodes = {
			UID_TEST1: new Node(),
			UID_TEST2: new Node(),
		};
	}

	async insert(uid, fragment, timestamp) {
		Object.assign(this.data, fragment);
		this.timestamp = timestamp;
	}

	async *getNodes() {
		for (const uid in this.nodes) {
			yield uid;
		}
	}

	async get(uid) {
		return await this.nodes[uid];
	}
}
