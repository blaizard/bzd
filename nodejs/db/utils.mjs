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

	static pagingFromParam(maxOrPaging, page = 0) {
		return typeof maxOrPaging == "object" ? maxOrPaging : { page: page, max: maxOrPaging };
	}

	static async makeFromList(data, maxOrPaging, callback = (item) => item) {
		const paging = CollectionPaging.pagingFromParam(maxOrPaging);
		const indexStart = paging.page * paging.max;
		const indexEnd = indexStart + paging.max;
		let slice = [];
		for (const item of data.slice(indexStart, indexEnd)) {
			slice.push(await callback(item));
		}
		return new CollectionPaging(slice, indexEnd >= data.length ? null : { page: paging.page + 1, max: paging.max });
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
			total > (paging.page + 1) * paging.max ? { page: paging.page + 1, max: paging.max } : null,
		);
	}

	static async *makeIterator(generator, maxOrPaging = 20) {
		do {
			const results = await generator(maxOrPaging);
			const data = results.data();
			for (const name in data) {
				yield [name, data[name]];
			}
			maxOrPaging = results.getNextPaging();
		} while (maxOrPaging !== null);
	}
}

export class AsyncInitialize {
	static async make(...args) {
		const instance = new this(...args);
		await instance._initialize();
		return instance;
	}

	/**
	 * Initialize (optional)
	 */
	async _initialize() {
		await this._initializeImpl();
	}
}
