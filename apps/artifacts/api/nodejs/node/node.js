import config from "#bzd/apps/artifacts/api/config.json" with { type: "json" };
import { HttpClient } from "#bzd/nodejs/core/http/client.js";
import { CollectionPaging } from "#bzd/nodejs/db/utils.js";
import { timestampMs } from "#bzd/nodejs/utils/timestamp.js";
import Utils from "#bzd/apps/artifacts/common/utils.js";
import { ArtifactsBase } from "#bzd/apps/artifacts/api/nodejs/common.js";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";

const Exception = ExceptionFactory("artifacts", "api");
const Log = LogFactory("artifacts", "api");

export class Node extends ArtifactsBase {
	constructor({ path = null, ...rest } = {}) {
		super(rest);
		this.path = [...(path ?? [])];
	}

	/// Generate the URI from the parameters.
	_makeURI(uid = null, volume = null, path = null) {
		const actualUid = uid ?? this.uid;
		Exception.assert(actualUid, "No UID were specified.");
		const subPath = ["data", ...this.path, ...(path ?? [])].map((s) => encodeURIComponent(s)).join("/");
		return (
			"/x/" + encodeURIComponent(volume ?? this.volume) + "/" + encodeURIComponent(actualUid) + "/" + subPath + "/"
		);
	}

	/// Publish data to a remote.
	///
	/// \param data The data to be published.
	/// \param uid The unique identifier of the node.
	/// \param volume The volume to which the data should be sent.
	/// \param path The path to publish to.
	async publish({ data, uid = null, volume = null, path = null }) {
		const uri = this._makeURI(uid, volume, path);
		await this._publish({
			uri: uri,
			data: [[[], [[timestampMs(), data]]]],
			isClientTimestamp: true,
		});
	}

	/// Publish a bulk of data to a remote.
	///
	/// \param uid The unique identifier of the node.
	/// \param volume The volume to which the data should be sent.
	/// \param path The path to publish to.
	/// \param isClientTimestamp If true, the timestamps given for each entry are the client timestamp
	///                          in milliseconds.
	/// \param callback Function invoked with a `publish(timestampMs, data)` helper.
	async publishBulk({ uid = null, volume = null, path = null, isClientTimestamp = true } = {}, callback) {
		const bulk = [];
		await callback(({ timestampMs, value, key = null, expires = null, unit = null }) => {
			let data = [timestampMs, value];
			if (expires) {
				data[2] = expires;
			}
			if (unit) {
				data[3] = unit;
			}
			bulk.push([key ?? [], [data]]);
		});
		if (bulk.length === 0) {
			return;
		}
		const uri = this._makeURI(uid, volume, path);
		await this._publish({
			uri: uri,
			data: bulk,
			isClientTimestamp: isClientTimestamp,
		});
	}

	async _publish(entry) {
		const headers = {};
		if (this.token) {
			headers.authorization = "basic " + this.token;
		}
		for (const [remote, retry, nbRetries] of this.remotes()) {
			try {
				const content = { data: entry.data };
				if (entry.isClientTimestamp) {
					content.timestamp = timestampMs();
				}
				const url = remote + entry.uri;
				await this.httpClient.post(url, { json: content, query: { bulk: 1 }, headers: headers });
				return;
			} catch (e) {
				// ignore.
			}
		}
		throw Exception.error("Unable to publish to any of the remotes.");
	}

	/// List all nodes from a remote.
	///
	/// \param remote The url of the remote.
	/// \param volume The name of the volume to be used.
	async list({ remote, token = null, volume = null }) {
		let nodes = [];
		let query = {};

		token ??= this.token;
		if (token) {
			query.t = token;
		}
		for await (const [_, data] of CollectionPaging.makeIterator(async (maxOrPaging) => {
			const result = await this.httpClient.post(remote + "/api/v1/list", {
				json: {
					path: [volume ?? this.volume],
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
	async get({ remote, uid, token = null, volume = null, path = null, children = 0, include = null, metadata = false }) {
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

		token ??= this.token;
		if (token) {
			query.t = token;
		}

		const result = await this.httpClient.get(
			remote +
				"/x/" +
				encodeURIComponent(volume ?? this.volume) +
				"/" +
				encodeURIComponent(uid) +
				Utils.keyToPath(path || []),
			{
				query: query,
				expect: "json",
			},
		);

		// Adjust the timestamp to the local server.
		if (metadata) {
			const timestampDiff = result.timestamp - timestampMs();
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
