import Base from "./storage.mjs";
import LogFactory from "../../core/log.mjs";
import ExceptionFactory from "../../core/exception.mjs";
import Cache from "../../core/cache.mjs";
import { CollectionPaging } from "../utils.mjs";
import { FetchFactory } from "../../core/fetch.mjs";

const Log = LogFactory("db", "storage", "docker-v2");
const Exception = ExceptionFactory("db", "storage", "docker-v2");

/**
 * File storage module
 */
export default class StorageDockerV2 extends Base {
	constructor(url, options = {}) {
		super();

		this.options = Object.assign(
			{
				authentication: null
			},
			options
		);

		this.cache = new Cache();

		// Handle authentication, this function will call a callback to generate the token
		this.cache.register("token", async (scope, prevToken, options) => {
			const result = await this.options.authentication(this.fetch, scope);
			// Default to 60s if omitted: https://docs.docker.com/registry/spec/auth/token/
			const expiresInS = "expires_in" in result ? result.expires_in : 60;
			options.timeout = (expiresInS * 1000) / 2;
			return result.token;
		});

		this.fetch = new FetchFactory(url, async (scope) => {
			let options = {};

			if (scope && this.options.authentication) {
				options = {
					authentication: {
						type: "bearer",
						token: await this.cache.get("token", scope)
					}
				};
			}

			return Object.assign(
				{
					expect: "json"
				},
				options
			);
		});

		Log.info("Using Docker v2 at '{}'.", url);
		this._initialize();
	}

	static makeFromGcr(keyContent, service = "gcr.io") {
		Exception.assert(typeof keyContent == "string", "keyContent must be a string: {:j}", keyContent);
		return new StorageDockerV2("https://" + service, {
			authentication: async (fetch, scope) => {
				return StorageDockerV2._authentication(fetch, service, scope, {
					authentication: {
						type: "basic",
						username: "_json_key",
						password: keyContent
					}
				});
			}
		});
	}

	static async _authentication(fetch, service, scope, fetchOptions) {
		return fetch.get(
			"/v2/token",
			Object.assign(
				{
					query: {
						service: service,
						scope: scope
					}
				},
				fetchOptions
			)
		);
	}

	async _initializeImpl() {
		await this.fetch.request("/v2/", {
			args: "registry:catalog:*",
			method: "get"
		});
	}

	async _listPagination(endpoint, maxOrPaging, scope, callback) {
		const paging = CollectionPaging.pagingFromParam(maxOrPaging);
		const [result, headers] = await this.fetch.request(
			endpoint,
			{
				args: scope,
				method: "get",
				query: {
					n: paging.max,
					last: paging.page === 0 ? undefined : paging.page
				}
			},
			/*includeAll*/ true
		);
		const data = callback(result);
		return new CollectionPaging(data, "link" in headers ? { page: data[data.length - 1].name, max: paging.max } : null);
	}

	async _listCatalog(maxOrPaging, includeMetadata) {
		return await this._listPagination("/v2/_catalog", maxOrPaging, "registry:catalog:*", (result) => {
			return includeMetadata
				? result.repositories.map((item) => ({
					name: item,
					type: "directory"
				  }))
				: result.repositories;
		});
	}

	async _listTags(imageName, maxOrPaging) {
		return await this._listPagination(
			"/v2/" + imageName + "/tags/list",
			maxOrPaging,
			"repository:" + imageName + ":pull",
			(result) => {
				return result.tags.map((item) => ({
					name: item,
					type: "directory"
				}));
			}
		);
	}

	async _listImpl(pathList, maxOrPaging, includeMetadata) {
		if (pathList.length == 0) {
			return await this._listCatalog(maxOrPaging, includeMetadata);
		}
		const imageName = pathList[0];
		if (pathList.length == 1) {
			return await this._listTags(imageName, maxOrPaging);
		}
		const tag = pathList[1];
		const result3 = await this.fetch.get("/v2/" + imageName + "/manifests/" + tag, {
			args: "repository:" + imageName + ":pull",
			headers: {
				Accept: "application/vnd.docker.distribution.manifest.v2+json"
			}
		});
		console.log(result3);
	}
}
