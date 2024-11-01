import config from "#bzd/apps/artifacts/api/config.json" with { type: "json" };
import HttpClient from "#bzd/nodejs/core/http/client.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";

export class Node {
	/// List all nodes from a remote.
	///
	/// \param remote The url of the remote.
	/// \param volume The name of the volume to be used.
	async list({ remote, volume = config.defaultNodeVolume }) {
		let nodes = [];
		for await (const [_, data] of CollectionPaging.makeIterator(async (maxOrPaging) => {
			const result = await HttpClient.post(remote + "/api/v1/list", {
				json: {
					path: [volume],
					paging: maxOrPaging,
				},
				expect: "json",
			});
			return new CollectionPaging(result.data, result.next);
		}, 50)) {
			nodes.push(data.name);
		}
		return nodes;
	}

	/// Get data from the nodes remote volumes.
	///
	/// \param remote The url of the remote.
	/// \param uid The uid of the node to be queried.
	/// \param volume The name of the volume to be used.
	/// \param path The path to be used. This must be a list of path segments.
	/// \param children The number of children depth level to be included.
	/// \param include The given path to be included. A list of list of path segments.
	async get({ remote, uid, volume = config.defaultNodeVolume, path = null, children = 0, include = null }) {
		let query = {};
		if (children) {
			query.children = children;
		}
		if (include) {
			query.include = include.map((paths) => paths.map(encodeURIComponent).join("/")).join(",");
		}

		const pathUrl = (path || []).map(encodeURIComponent).join("/");
		const result = await HttpClient.get(
			remote + "/x/" + encodeURIComponent(volume) + "/" + encodeURIComponent(uid) + "/" + pathUrl,
			{
				query: query,
				expect: "json",
			},
		);

		return result;
	}

	/// Get data from the nodes as a tree.
	///
	/// This is a wrapper around get(...) that changes the result which is originally a list
	/// of tuples (keys/value) into a tree.
	async getAsTree(options) {
		const result = await this.get(options);

		let tree = {};
		for (const [key, value] of result.data) {
			const last = key.pop();
			const object = key.reduce((r, segment) => {
				r[segment] ??= {};
				return r[segment];
			}, tree);
			object[last] = value;
		}
		result.data = tree;

		return result;
	}
}
