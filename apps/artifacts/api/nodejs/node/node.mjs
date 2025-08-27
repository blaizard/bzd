import config from "#bzd/apps/artifacts/api/config.json" with { type: "json" };
import HttpClient from "#bzd/nodejs/core/http/client.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
import Utils from "#bzd/apps/artifacts/common/utils.mjs";

export class Node {
	/// List all nodes from a remote.
	///
	/// \param remote The url of the remote.
	/// \param volume The name of the volume to be used.
	async list({ remote, token = null, volume = config.defaultNodeVolume }) {
		let nodes = [];
		let query = {};
		if (token) {
			query.t = token;
		}
		for await (const [_, data] of CollectionPaging.makeIterator(async (maxOrPaging) => {
			const result = await HttpClient.post(remote + "/api/v1/list", {
				json: {
					path: [volume],
					paging: maxOrPaging,
				},
				query: query,
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
	/// \param metadata Include metadata with each values.
	async get({
		remote,
		uid,
		token = null,
		volume = config.defaultNodeVolume,
		path = null,
		children = 0,
		include = null,
		metadata = false,
	}) {
		let query = {};
		if (children) {
			query.children = children;
		}
		if (metadata) {
			query.metadata = 1;
		}
		if (include) {
			query.include = include.map(Utils.keyToPath).join(",");
		}
		if (token) {
			query.t = token;
		}

		const result = await HttpClient.get(
			remote + "/x/" + encodeURIComponent(volume) + "/" + encodeURIComponent(uid) + Utils.keyToPath(path || []),
			{
				query: query,
				expect: "json",
			},
		);

		// Adjust the timestamp to the local server.
		if (metadata) {
			const timestampDiff = result.timestamp - Utils.timestampMs();
			result.data = result.data.map(([key, value]) => {
				const [t, ...rest] = value;
				return [key, [t - timestampDiff, ...rest]];
			});
		}

		return result;
	}

	/// Convert the data from a flat view to a tree.
	///
	/// \note Same keys will be overwritten, keeping the latest one only.
	static toTree(data) {
		let tree = {};
		for (const [key, value] of data) {
			const last = key.pop();
			const object = key.reduce((r, segment) => {
				r[segment] ??= {};
				return r[segment];
			}, tree);
			object[last] = value;
		}
		return tree;
	}

	/// Get data from the nodes as a tree.
	///
	/// This is a wrapper around get(...) that changes the result which is originally a list
	/// of tuples (keys/value) into a tree.
	async getAsTree(options) {
		const result = await this.get(options);

		result.data = Node.toTree(result.data);

		return result;
	}
}
