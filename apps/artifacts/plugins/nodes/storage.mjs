import Storage from "#bzd/nodejs/db/storage/storage.mjs";

import LogFactory from "#bzd/nodejs/core/log.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";

const Log = LogFactory("apps", "plugin", "nodes");

export default class StorageBzd extends Storage {
	constructor(nodes) {
		super();
		this.nodes = nodes;
	}

	async _initialize() {}

	async _listNodes(maxOrPaging) {
		let uids = [];
		for await (const uid of this.nodes.getNodes()) {
			uids.push(uid);
		}
		return await CollectionPaging.makeFromList(uids, maxOrPaging, (uid) => {
			return Permissions.makeEntry(
				{
					name: uid,
				},
				{ read: true, list: true },
			);
		});
	}

	async _listCategories(uid, paths, maxOrPaging) {
		const node = await this.nodes.get(uid);
		const data = await node.get(paths);
		return await CollectionPaging.makeFromList(Object.keys(data), maxOrPaging, (name) => {
			return Permissions.makeEntry(
				{
					name: name,
				},
				{ read: true, list: data[name] && data[name].constructor == Object },
			);
		});
	}

	async _listImpl(pathList, maxOrPaging, includeMetadata) {
		if (pathList.length == 0) {
			return await this._listNodes(maxOrPaging);
		}
		return await this._listCategories(pathList[0], pathList.slice(1), maxOrPaging);
	}
}
