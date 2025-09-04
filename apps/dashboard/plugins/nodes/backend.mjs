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
					children: 3,
					include: [
						["active"],
						["battery"],
						["cpu"],
						["gpu"],
						["disk"],
						["memory"],
						["network"],
						["io"],
						["temperature"],
						["version"],
						["uptime"],
					],
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
		// Merge similar nodes (needed when the same node is on 2 different remote for example).
		let clusters = {};
		for (const { key, data } of listOfNodes) {
			clusters[key] ??= [];
			clusters[key] = clusters[key].concat(data);
		}

		// Set to tree and to list.
		const formatted = Object.entries(clusters).map(([key, data]) => Object.assign({ key: key }, Node.toTree(data)));

		// Sort to ensure node order consistency on the dashboard.
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
