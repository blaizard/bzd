import Timeseries from "./timeseries.mjs";
import { FetchFactory } from "../../core/fetch.mjs";
import { CollectionPaging } from "../utils.mjs";
import ExceptionFactory from "../../core/exception.mjs";

const Exception = ExceptionFactory("db", "timeseries", "elasticsearch");
export default class TimeseriesElasticsearch extends Timeseries {
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

	async insert(bucket, timestamp, data) {
		await this.fetch.request("/" + encodeURIComponent(bucket) + "/_doc", {
			method: "post",
			data: {
				date: timestamp,
				data: data,
			},
		});
	}

	async _search(bucket, maxOrPaging, query) {
		const paging = typeof maxOrPaging == "object" ? maxOrPaging : { page: 0, max: maxOrPaging };
		const result = await this.fetch.request(
			"/" + encodeURIComponent(bucket) + "/_search?size=" + paging.max + "&from=" + paging.page * paging.max,
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

		const total = result.hits.total.value;
		return new CollectionPaging(
			result.hits.hits.map((item) => [item._source.date, item._source.data]),
			total > (paging.page + 1) * paging.max ? { page: paging.page + 1, max: paging.max } : null
		);
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
