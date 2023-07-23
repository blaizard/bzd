import Timeseries from "./timeseries.mjs";
import { HttpClientFactory, HttpClientException } from "../../core/http/client.mjs";
import { CollectionPaging } from "../utils.mjs";
import ExceptionFactory from "../../core/exception.mjs";
import LogFactory from "../../core/log.mjs";

const Exception = ExceptionFactory("db", "timeseries", "elasticsearch");
const Log = LogFactory("db", "timeseries", "elasticsearch");

export default class TimeseriesElasticsearch extends Timeseries {
	constructor(host, options) {
		super();
		this.options = Object.assign(
			{
				user: null,
				key: null,
				prefix: "timeseries_",
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
			method: "get",
		});
		Exception.assert("version" in result, "Unexpected response: {:j}", result);
	}

	_bucketToURI(bucket) {
		const indexName = this.options.prefix + encodeURIComponent(bucket);
		// Elastic search only accepts lower case index names
		return indexName.toLowerCase();
	}

	async _insertImpl(bucket, timestamp, data) {
		await this.fetch.request("/" + this._bucketToURI(bucket) + "/_doc", {
			method: "post",
			json: {
				date: timestamp,
				data: data,
			},
		});
	}

	async _search(bucket, maxOrPaging, query) {
		const paging = CollectionPaging.pagingFromParam(maxOrPaging);

		try {
			const result = await this.fetch.request(
				"/" + this._bucketToURI(bucket) + "/_search?size=" + paging.max + "&from=" + paging.page * paging.max,
				{
					method: "post",
					json: {
						query: query,
						sort: {
							date: "desc",
						},
					},
				}
			);
			Exception.assert("hits" in result && "hits" in result.hits, "Result malformed: {:j}", result);

			return CollectionPaging.makeFromTotal(
				result.hits.hits.map((item) => [item._source.date, item._source.data]),
				paging,
				result.hits.total.value
			);
		}
		catch (e) {
			if (e instanceof HttpClientException) {
				if (e.code == 404 /*Not Found*/) {
					return new CollectionPaging([]);
				}
			}
			throw e;
		}
	}

	async _listImpl(bucket, maxOrPaging) {
		return await this._search(bucket, maxOrPaging, {
			match_all: {},
		});
	}

	async _listUntilTimestampImpl(bucket, timestamp, maxOrPaging) {
		return await this._search(bucket, maxOrPaging, {
			range: {
				date: {
					gte: timestamp,
				},
			},
		});
	}
}
