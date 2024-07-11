import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import HttpClient from "#bzd/nodejs/core/http/client.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";

const Exception = ExceptionFactory("plugin", "nodes");
const Log = LogFactory("plugin", "nodes");

export default class Nodes {
	constructor(config) {
		this.config = config;
	}

	static register(cache) {
		cache.register(
			"nodes.list",
			async function (url, volume) {
				let nodes = [];
				for await (const [_, data] of CollectionPaging.makeIterator(async (maxOrPaging) => {
					const result = await HttpClient.post(url + "/api/v1/list", {
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
			},
			{ timeout: 60 * 1000 },
		);
	}

	async fetch(cache) {
		return await cache.get("nodes.list", this.config["nodes.url"], this.config["nodes.volume"]);
	}
}
