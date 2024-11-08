import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import HttpClient from "#bzd/nodejs/core/http/client.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
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
				const result = await node.getAsTree({
					remote: url,
					uid: uid,
					volume: volume,
					token: token,
					path: ["data"],
					children: 2,
					include: [["battery"], ["cpu"], ["gpu"], ["disk"], ["memory"], ["temperature"]],
				});

				if (Object.keys(result.data).length == 0) {
					return null;
				}

				return Object.assign(
					{
						key: uid,
					},
					result.data,
				);
			},
			{ timeout: 1000 },
		);
	}

	async fetch(cache) {
		const nodes = await cache.get(
			"nodes.list",
			this.config["nodes.url"],
			this.config["nodes.token"],
			this.config["nodes.volume"],
		);

		const promises = nodes.map((node) =>
			cache.get("nodes.data", this.config["nodes.url"], this.config["nodes.token"], this.config["nodes.volume"], node),
		);
		const results = await Promise.all([...promises]);

		return results.filter((entry) => entry !== null);
	}
}
