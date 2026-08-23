import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import Cache2 from "#bzd/nodejs/core/cache2.js";
import { Node } from "#bzd/apps/artifacts/api/nodejs/node/node.js";

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
			async (key, context, { url, token, volume }) => {
				return await node.list({ remote: url, volume: volume, token: token });
			},
			{ timeoutMs: 60 * 1000 },
		);

		cache.register(
			"nodes.data",
			async (key, context, { url, token, volume, uid }) => {
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
						["downtime"],
						["leases"],
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
			{ timeoutMs: 1000 },
		);
	}

	/// Fetch all node remotes for a list of nodes.
	///
	/// \return An array of dictionary which looks like:
	///         { nodes: [node1, node2, ...], url: ..., volume: ... }
	async fetchList(cache) {
		const promises = this.config["nodes.remotes"].map((remote) => {
			const url = remote.url;
			const token = remote.token;
			const volume = remote.volume;
			return cache.get("nodes.list", Cache2.arrayOfStringToKey([url, volume]), {
				url: url,
				token: token,
				volume: volume,
			});
		});
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
			data.nodes.map((node) => {
				const url = data.url;
				const token = data.token;
				const volume = data.volume;
				const uid = node;
				return cache.get("nodes.data", Cache2.arrayOfStringToKey([url, volume, uid]), {
					url: url,
					token: token,
					volume: volume,
					uid: uid,
				});
			}),
		);
		const results = await Promise.all(
			listOfPromises.reduce((accumulator, currentValue) => accumulator.concat(currentValue), []),
		);
		return this.mergeAndFormatNodes(results.filter((entry) => entry !== null));
	}
}
