import Base from "./storage.mjs";
import LogFactory from "../../core/log.mjs";
import { CollectionPaging } from "../utils.mjs";
import { FetchFactory } from "../../core/fetch.mjs";

const Log = LogFactory("db", "storage", "docker-v2");

/**
 * File storage module
 */
export default class StorageDockerV2 extends Base {
	constructor(url, options) {
		super();

		this.options = Object.assign({}, options);

		let fetchOptions = {
			expect: "json",
		};
		this.fetch = new FetchFactory(url, fetchOptions);

		Log.info("Using Docker v2 at '{}'.", this.url);
		this._initialize();
	}

	async _initializeImpl() {
		await this.fetch.request("/v2/", {
			method: "get",
		});
	}

	async _listPagination(endpoint, maxOrPaging, callback) {
		const paging = CollectionPaging.pagingFromParam(maxOrPaging);
		const [result, headers] = await this.fetch.request(
			endpoint,
			{
				method: "get",
				query: {
					n: paging.max,
					last: paging.page === 0 ? undefined : paging.page,
				},
			},
			/*includeHeaders*/ true
		);
		const data = callback(result);
		return new CollectionPaging(data, "link" in headers ? { page: data[data.length - 1].name, max: paging.max } : null);
	}

	async _listCatalog(maxOrPaging, includeMetadata) {
		return await this._listPagination("/v2/_catalog", maxOrPaging, (result) => {
			return includeMetadata
				? result.repositories.map((item) => ({
					name: item,
					type: "directory",
					path: encodeURIComponent(item),
				  }))
				: result.repositories;
		});
	}

	async _listTags(imageName, maxOrPaging) {
		return await this._listPagination("/v2/" + imageName + "/tags/list", maxOrPaging, (result) => {
			return result.tags.map((item) => ({
				name: item,
				type: "directory",
			}));
		});
	}

	async _listImpl(path, maxOrPaging, includeMetadata) {
		const pathSplitted = path.split("/").filter((item) => Boolean(item.length));
		if (pathSplitted.length == 0) {
			return await this._listCatalog(maxOrPaging, includeMetadata);
		}
		const imageName = pathSplitted[0];
		if (pathSplitted.length == 1) {
			return await this._listTags(imageName, maxOrPaging);
		}

		const result3 = await this.fetch.request("/v2/apps/dashboard/manifests/latest", {
			method: "get",
			headers: {
				Accept: "application/vnd.docker.distribution.manifest.v2+json",
			},
		});
		console.log(result3);
	}
}
