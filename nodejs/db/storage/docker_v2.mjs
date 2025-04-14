import Cache from "../../core/cache.mjs";
import ExceptionFactory from "../../core/exception.mjs";
import { HttpClientFactory } from "../../core/http/client.mjs";
import LogFactory from "../../core/log.mjs";
import { CollectionPaging } from "../utils.mjs";

import Permissions from "./permissions.mjs";
import { Storage, FileNotFoundError } from "./storage.mjs";

const Log = LogFactory("db", "storage", "docker-v2");
const Exception = ExceptionFactory("db", "storage", "docker-v2");

/**
 * File storage module
 */
export default class StorageDockerV2 extends Storage {
	constructor(url, options = {}) {
		super(
			Object.assign(
				{
					authentication: null,
				},
				options,
			),
		);

		this.url = url;
		this.cache = new Cache();

		// Handle authentication, this function will call a callback to generate the token
		this.cache.register("token", async (scope, prevToken, options) => {
			const startTime = new Date();
			const result = await this.options.authentication.call(this, scope);
			const elpasedTimeS = (new Date() - startTime) / 1000;

			// Default to 60s if omitted: https://docs.docker.com/registry/spec/auth/token/
			const expiresInS = "expires_in" in result ? result.expires_in : 60;
			options.timeout = (expiresInS - elpasedTimeS * 2) * 1000;

			return result.token;
		});

		this.fetch = new HttpClientFactory(url, async (scope) => {
			let options = {};

			if (scope && this.options.authentication) {
				options = {
					authentication: {
						type: "bearer",
						token: await this.cache.get("token", scope),
					},
				};
			}

			return Object.assign(
				{
					expect: "json",
				},
				options,
			);
		});

		Log.info("Using Docker v2 at '{}'.", this.url);
	}

	static async makeFromGcr(keyContent, service = "gcr.io") {
		Exception.assert(typeof keyContent == "string", "keyContent must be a string: {:j}", keyContent);
		return await StorageDockerV2.make("https://" + service, {
			authentication: async function (scope) {
				return await this.authenticationScope(service, scope, {
					authentication: {
						type: "basic",
						username: "_json_key",
						password: keyContent,
					},
				});
			},
		});
	}

	async authenticationScope(service, scope, fetchOptions) {
		return this.fetch.get(
			"/v2/token",
			Object.assign(
				{
					query: {
						service: service,
						scope: scope,
					},
				},
				fetchOptions,
			),
		);
	}

	async _initialize() {
		await this.fetch.request("/v2/", {
			args: "registry:catalog:*",
			method: "get",
		});
	}

	async _listPagination(endpoint, maxOrPaging, scope, callback) {
		const paging = CollectionPaging.pagingFromParam(maxOrPaging);
		const [result, headers] = await this.fetch.request(endpoint, {
			args: scope,
			method: "get",
			query: {
				n: paging.max,
				last: paging.page === 0 ? undefined : paging.page,
			},
			includeAll: true,
		});
		const data = callback(result);
		return new CollectionPaging(data, "link" in headers ? { page: data[data.length - 1].name, max: paging.max } : null);
	}

	async _listCatalog(maxOrPaging) {
		return await this._listPagination("/v2/_catalog", maxOrPaging, "registry:catalog:*", (result) => {
			return result.repositories.map((item) =>
				Permissions.makeEntry(
					{
						name: item,
						type: "directory",
					},
					{
						list: true,
					},
				),
			);
		});
	}

	async _listTags(imageName, maxOrPaging) {
		return await this._listPagination(
			"/v2/" + imageName + "/tags/list",
			maxOrPaging,
			"repository:" + imageName + ":pull",
			(result) => {
				return result.tags.map((item) =>
					Permissions.makeEntry(
						{
							name: item,
							type: "directory",
						},
						{
							list: true,
						},
					),
				);
			},
		);
	}

	async _listLayers(imageName, tag, maxOrPaging) {
		const [result, headers] = await this.fetch.get("/v2/" + imageName + "/manifests/" + tag, {
			args: "repository:" + imageName + ":pull",
			headers: {
				Accept: "application/vnd.docker.distribution.manifest.v2+json, application/vnd.oci.image.manifest.v1+json",
			},
			includeAll: true,
		});
		let data = result.layers.map((layer) => {
			return Permissions.makeEntry(
				{
					name: layer.digest.replace(/^.*:/, ""),
					size: layer.size,
					type: "layer",
					digest: layer.digest,
				},
				{
					read: true,
				},
			);
		});

		const resultDigest = await this.fetch.get("/v2/" + imageName + "/blobs/" + result.config.digest, {
			args: "repository:" + imageName + ":pull",
		});
		data.unshift(
			Permissions.makeEntry(
				{
					name: "manifest.json",
					size: result.config.size,
					type: "manifest",
					digest: result.config.digest,
					architecture: resultDigest.architecture,
					os: resultDigest.os,
					created: resultDigest.created,
					author: resultDigest.author,
				},
				{
					read: true,
				},
			),
		);

		data.unshift(
			Permissions.makeEntry({
				name: ".metadata",
				type: "metadata",
				digest: headers["docker-content-digest"],
			}),
		);

		return await CollectionPaging.makeFromList(data, maxOrPaging);
	}

	async _listImpl(pathList, maxOrPaging, includeMetadata) {
		if (pathList.length == 0) {
			const results = await this._listCatalog(maxOrPaging, includeMetadata);
			return includeMetadata ? results : results.map((item) => item.name);
		}
		const imageName = pathList[0];
		if (pathList.length == 1) {
			const results = await this._listTags(imageName, maxOrPaging);
			return includeMetadata ? results : results.map((item) => item.name);
		}
		const tag = pathList[1];
		if (pathList.length == 2) {
			const results = await this._listLayers(imageName, tag, maxOrPaging);
			return includeMetadata ? results : results.map((item) => item.name);
		}
		throw new FileNotFoundError(pathList.join("/"));
	}
}
