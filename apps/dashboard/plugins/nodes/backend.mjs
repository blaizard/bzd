import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import { Node } from "#bzd/apps/artifacts/api/nodejs/node/node.mjs";

const Exception = ExceptionFactory("plugin", "nodes");
const Log = LogFactory("plugin", "nodes");

export default class Nodes {
	constructor(config) {
		this.config = config;
	}

	static register(cache) {
		const node = new Node();

		cache.register(
			"nodes.list",
			async function (url, token, volume) {
				return await node.list({ remote: url, volume: volume, token: token });
			},
			{ timeout: 60 * 1000 },
		);

		cache.register(
			"nodes.data",
			async (url, token, volume, uid) => {
				const result = await node.get({
					remote: url,
					uid: uid,
					volume: volume,
					token: token,
					path: ["data"],
					children: 2,
					include: [["battery"], ["cpu"], ["gpu"], ["disk"], ["memory"], ["temperature"]],
					metadata: true,
				});

				if (result.data.length == 0) {
					return null;
				}

				return {
					key: uid,
					data: result.data,
				};
			},
			{ timeout: 1000 },
		);
	}

	/// Fetch all node remotes for a list of nodes.
	///
	/// \return An array of dictionary which looks like:
	///         { nodes: [node1, node2, ...], url: ..., volume: ... }
	async fetchList(cache) {
		const promises = this.config["nodes.remotes"].map((remote) =>
			cache.get("nodes.list", remote.url, remote.token, remote.volume),
		);
		const listOfNodes = await Promise.all(promises);
		return listOfNodes.map((nodes, index) => {
			return Object.assign(
				{
					nodes: nodes,
				},
				this.config["nodes.remotes"][index],
			);
		});
	}

	/// Merge similar nodes together and keep the ones with the newest timestamp.
	mergeAndFormatNodes(listOfNodes) {
		let clusters = {};
		// Merge similar nodes.
		for (const { key, data } of listOfNodes) {
			clusters[key] ??= [];
			clusters[key] = clusters[key].concat(data);
		}
		// Sort by timestamp.
		const sortedTuple = Object.entries(clusters).map(([key, data]) => {
			return [
				key,
				data
					.sort(([key1, [t1, ...rest1]], [key2, [t2, ...rest2]]) => t1 - t2)
					.map(([key, [t, ...rest]]) => [key, ...rest]),
			];
		});
		// Set to tree and to list.
		const formatted = sortedTuple.map(([key, data]) => Object.assign({ key: key }, Node.toTree(data)));
		return formatted.sort((a, b) => a.key.localeCompare(b.key));
	}

	async fetch(cache) {
		const listOfNodes = await this.fetchList(cache);
		const listOfPromises = listOfNodes.map((data) =>
			data.nodes.map((node) => cache.get("nodes.data", data.url, data.token, data.volume, node)),
		);
		const results = await Promise.all(
			listOfPromises.reduce((accumulator, currentValue) => accumulator.concat(currentValue), []),
		);
		return this.mergeAndFormatNodes(results.filter((entry) => entry !== null));
	}
}
