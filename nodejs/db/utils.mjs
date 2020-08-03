import Event from "../core/event.mjs";

export class CollectionPaging {
	constructor(data, nextPaging = null) {
		this._data = data;
		this._nextPaging = nextPaging;
	}

	data() {
		return this._data;
	}

	isLast() {
		return !this._nextPaging;
	}

	getNextPaging() {
		return this._nextPaging;
	}

	static pagingFromParam(maxOrPaging) {
		return typeof maxOrPaging == "object" ? maxOrPaging : { page: 0, max: maxOrPaging };
	}

	static async makeFromList(data, maxOrPaging, callback = (item) => item) {
		const paging = CollectionPaging.pagingFromParam(maxOrPaging);
		const indexStart = paging.page * paging.max;
		const indexEnd = indexStart + paging.max;
		let slice = [];
		for (const item of data.slice(indexStart, indexEnd)) {
			slice.push(await callback(item));
		}
		return new CollectionPaging(slice, indexEnd > data.length ? null : { page: paging.page + 1, max: paging.max });
	}

	static async makeFromObject(data, maxOrPaging, callback = (item) => item) {
		const keysList = Object.keys(data).sort();
		let result = await this.makeFromList(keysList, maxOrPaging, callback);
		result._data = result._data.reduce((obj, key) => {
			obj[key] = data[key];
			return obj;
		}, {});
		return result;
	}

	static makeFromTotal(data, paging, total) {
		return new CollectionPaging(
			data,
			total > (paging.page + 1) * paging.max ? { page: paging.page + 1, max: paging.max } : null
		);
	}
}

export class AsyncInitialize {
	constructor() {
		this.event = new Event({
			ready: { proactive: true },
			error: { proactive: true },
		});
	}

	/**
	 * Initialize
	 */
	async _initialize() {
		try {
			await this._initializeImpl();
			this.event.trigger("ready");
		}
		catch (e) {
			this.event.trigger("error", e);
			throw e;
		}
	}

	/**
	 * Initialization function (optional)
	 */
	async _initializeImpl() {}

	/**
	 * This function waits until the module is ready
	 */
	async waitReady() {
		return await this.event.waitUntil("ready");
	}

	/**
	 * Check if ready
	 */
	isReady() {
		return this.event.is("ready");
	}
}
