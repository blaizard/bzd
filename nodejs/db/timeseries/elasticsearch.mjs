import Timeseries from "./timeseries.mjs";
import { FetchFactory, FetchException } from "../../core/fetch.mjs";
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
		this.fetch = new FetchFactory(host, fetchOptions);
		this._initialize();
	}

	_bucketToURI(bucket) {
		return this.options.prefix + encodeURIComponent(bucket);
	}

	async insert(bucket, timestamp, data) {
		await this.fetch.request("/" + this._bucketToURI(bucket) + "/_doc", {
			method: "post",
			data: {
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
					data: {
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
			if (e instanceof FetchException) {
				if (e.code == 404 /*Not Found*/) {
					return new CollectionPaging([]);
				}
			}
			throw e;
		}
	}

	async list(bucket, maxOrPaging = 10) {
		return await this._search(bucket, maxOrPaging, {
			match_all: {},
		});
	}

	async listUntilTimestamp(bucket, timestamp, maxOrPaging = 10) {
		return await this._search(bucket, maxOrPaging, {
			range: {
				date: {
					gte: timestamp,
				},
			},
		});
	}
}
