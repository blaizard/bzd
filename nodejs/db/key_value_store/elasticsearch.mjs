import KeyValueStore from "./key_value_store.mjs";
import { FetchFactory, FetchException } from "../../core/fetch.mjs";
import ExceptionFactory from "../../core/exception.mjs";

const Exception = ExceptionFactory("db", "kvs", "elasticsearch");

/**
 * Kay valud store adapater to elastic search DB
 */
export default class KeyValueStoreElasticsearch extends KeyValueStore {
	constructor(host, options) {
		super();
		this.options = Object.assign(
			{
				user: null,
				key: null,
			},
			options
		);

		let fetchOptions = {
			expect: "json",
		};

		if (this.options.user !== null) {
			fetchOptions.authentication = {
				type: "basic",
				username: this.options.user,
				password: this.options.key,
			};
		}

		this.fetch = new FetchFactory(host, fetchOptions);
		this._initialize();
	}

	async set(bucket, key, value) {
		let endpoint = "/" + encodeURIComponent(bucket) + "/_doc/";
		if (key !== null) {
			endpoint += encodeURIComponent(key);
		}
		const result = await this.fetch.request(endpoint, {
			method: "post",
			data: value,
		});
		Exception.assert(["created", "updated"].includes(result.result), "Unexpected result, received: {}", result.result);
		Exception.assert("_id" in result, "Response is malformed: {:j}", result);
		return result._id;
	}

	async get(bucket, key, defaultValue = undefined) {
		try {
			const result = await this.fetch.request("/" + encodeURIComponent(bucket) + "/_doc/" + encodeURIComponent(key), {
				method: "get",
			});
			Exception.assert("_source" in result, "Response is malformed: {:j}", result);
			return result._source;
		} catch (e) {
			if (e instanceof FetchException) {
				if (e.code == 404 /*Note Found*/) {
					return defaultValue;
				}
			}
		}
	}

	async delete(bucket, key) {
		const result = await this.fetch.request("/" + encodeURIComponent(bucket) + "/_doc/" + encodeURIComponent(key), {
			method: "delete",
		});
		Exception.assert(result._shards.failed === 0, "Delete operation failed: {:j}", result);
	}

	async _search(bucket, maxOrPaging, query) {
		const paging = typeof maxOrPaging == "object" ? maxOrPaging : { page: 0, max: maxOrPaging };
		const result = await this.fetch.request(
			"/" + encodeURIComponent(bucket) + "/_search?size=" + paging.max + "&from=" + paging.page * paging.max,
			{
				method: "post",
				data: {
					query: query,
				},
			}
		);
		Exception.assert("hits" in result && "hits" in result.hits, "Result malformed: {:j}", result);
		return result.hits.hits.reduce((obj, item) => {
			obj[item._id] = item._source;
			return obj;
		}, {});
	}

	async list(bucket, maxOrPaging = 10) {
		return await this._search(bucket, maxOrPaging, {
			match_all: {}, // eslint-disable-line
		});
	}

	async listMatch(bucket, subKey, value, maxOrPaging = 10) {
		return await this._search(bucket, maxOrPaging, {
			term: {
				[subKey]: {
					value: value,
					boost: 1,
				},
			},
		});
	}
}
