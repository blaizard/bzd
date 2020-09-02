import KeyValueStore from "./key_value_store.mjs";
import { CollectionPaging } from "../utils.mjs";
import { HttpClientFactory, HttpClientException } from "../../core/http/client.mjs";
import ExceptionFactory from "../../core/exception.mjs";
import LogFactory from "../../core/log.mjs";

const Exception = ExceptionFactory("db", "kvs", "elasticsearch");
const Log = LogFactory("db", "kvs", "elasticsearch");

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
				prefix: "kvs_"
			},
			options
		);

		let fetchOptions = {
			expect: "json"
		};

		if (this.options.user !== null) {
			fetchOptions.authentication = {
				type: "basic",
				username: this.options.user,
				password: this.options.key
			};
		}

		Log.info(
			"Using elasticsearch key value store DB at '{}' (prefix: {}, authentication: {}).",
			host,
			this.options.prefix,
			"authentication" in fetchOptions ? "on" : "off"
		);
		this.fetch = new HttpClientFactory(host, fetchOptions);
	}

	async _initialize() {
		const result = await this.fetch.request("/", {
			method: "get"
		});
		Exception.assert("version" in result, "Unexpected response: {:j}", result);
	}

	_bucketToURI(bucket) {
		return this.options.prefix + encodeURIComponent(bucket);
	}

	async _setImpl(bucket, key, value, query = {}) {
		let endpoint = "/" + this._bucketToURI(bucket) + "/_doc/";
		if (key !== null) {
			endpoint += encodeURIComponent(key);
		}
		const result = await this.fetch.request(endpoint, {
			method: "post",
			json: value,
			query: query
		});
		Exception.assert(["created", "updated"].includes(result.result), "Unexpected result, received: {}", result.result);
		Exception.assert("_id" in result, "Response is malformed: {:j}", result);
		return result._id;
	}

	async _getImpl(bucket, key, defaultValue, includeVersion = false) {
		try {
			const result = await this.fetch.request("/" + this._bucketToURI(bucket) + "/_doc/" + encodeURIComponent(key), {
				method: "get"
			});
			Exception.assert("_source" in result, "Response is malformed: {:j}", result);
			if (includeVersion) {
				return { value: result._source, version: result._version };
			}
			return result._source;
		}
		catch (e) {
			if (e instanceof HttpClientException) {
				if (e.code == 404 /*Not Found*/) {
					if (includeVersion) {
						return { value: defaultValue, version: undefined };
					}
					return defaultValue;
				}
			}
			throw e;
		}
	}

	async _updateImpl(bucket, key, modifier, defaultValue) {
		// Re-iterate until there is no conflict
		let maxConflicts = 100;
		do {
			const { value, version } = await this._getImpl(bucket, key, defaultValue, /*includeVersion*/ true);
			const updatedValue = await modifier(value);
			try {
				return await this._setImpl(bucket, key, updatedValue, {
					version: version
				});
			}
			catch (e) {
				if (e instanceof HttpClientException) {
					if (e.code == 409 /*Conflict*/ && version !== undefined) {
						continue;
					}
				}
				throw e;
			}
		} while (maxConflicts--);

		Exception.unreachable(
			"Too many conflicts have occured with update request ({}, {}), something is wrong, aborting.",
			bucket,
			key
		);
	}

	async _countImpl(bucket) {
		try {
			const result = await this.fetch.request("/" + this._bucketToURI(bucket) + "/_count", {
				method: "post",
				json: {
					query: {
						match_all: {}
					}
				}
			});
			Exception.assert("count" in result, "Result malformed: {:j}", result);
			return result.count;
		}
		catch (e) {
			if (e instanceof HttpClientException) {
				if (e.code == 404 /*Not Found*/) {
					return 0;
				}
			}
			throw e;
		}
	}

	async _search(bucket, maxOrPaging, query) {
		const paging = CollectionPaging.pagingFromParam(maxOrPaging);
		try {
			const result = await this.fetch.request(
				"/" + this._bucketToURI(bucket) + "/_search?size=" + paging.max + "&from=" + paging.page * paging.max,
				{
					method: "post",
					json: {
						query: query
					}
				}
			);
			Exception.assert("hits" in result && "hits" in result.hits, "Result malformed: {:j}", result);

			return CollectionPaging.makeFromTotal(
				result.hits.hits.reduce((obj, item) => {
					obj[item._id] = item._source;
					return obj;
				}, {}),
				paging,
				result.hits.total.value
			);
		}
		catch (e) {
			if (e instanceof HttpClientException) {
				if (e.code == 404 /*Not Found*/) {
					return new CollectionPaging({});
				}
			}
			throw e;
		}
	}

	async _listImpl(bucket, maxOrPaging) {
		return await this._search(bucket, maxOrPaging, {
			match_all: {} // eslint-disable-line
		});
	}

	async _listMatchImpl(bucket, subKey, value, maxOrPaging) {
		return await this._search(bucket, maxOrPaging, {
			term: {
				[subKey]: {
					value: value,
					boost: 1
				}
			}
		});
	}

	async _deleteImpl(bucket, key) {
		const result = await this.fetch.request("/" + this._bucketToURI(bucket) + "/_doc/" + encodeURIComponent(key), {
			method: "delete"
		});
		Exception.assert(result._shards.failed === 0, "Delete operation failed: {:j}", result);
	}
}
