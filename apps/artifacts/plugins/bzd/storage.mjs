import Storage from "#bzd/nodejs/db/storage/storage.mjs";

import LogFactory from "#bzd/nodejs/core/log.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";

const Log = LogFactory("apps", "plugin", "bzd");

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

	async _listCategories(uid, maxOrPaging) {
		const node = await this.nodes.get(uid);
		return await CollectionPaging.makeFromList(await node.getCategories(), maxOrPaging, (name) => {
			return Permissions.makeEntry(
				{
					name: name,
				},
				{ read: true },
			);
		});
	}

	async _listImpl(pathList, maxOrPaging, includeMetadata) {
		if (pathList.length == 0) {
			return await this._listNodes(maxOrPaging);
		} else if (pathList.length == 1) {
			return await this._listCategories(pathList[0], maxOrPaging);
		}

		return new CollectionPaging([]);
	}
}
