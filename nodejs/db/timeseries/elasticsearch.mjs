import Timeseries from "./timeseries.mjs";
import { FetchFactory } from "../../core/fetch.mjs";

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
	/*
	 *async insert(bucket, timestamp, data) {
	 *}
	 *
	 *async list(bucket, maxOrPaging) {
	 *}
	 *
	 *async listUntilTimestamp(bucket, timestamp, maxOrPaging) {
	 *}
	 */
}
