import ExceptionFactory from "../core/exception.mjs";

const Exception = ExceptionFactory("db", "utils");

export class CollectionPaging {
	constructor(data, nextPaging = null) {
		this._data = data;
		this._nextPaging = nextPaging;
	}

	data() {
		return this._data;
	}

	isLast() {
		return !Boolean(this._nextPaging);
	}

	getNextPaging() {
		return this._nextPaging;
	}

	static makeFromList(data, maxOrPaging) {
		const paging = (typeof maxOrPaging == "object") ? maxOrPaging : {page: 0, max: maxOrPaging};
		const indexStart = paging.page * paging.max;
		const indexEnd = indexStart + paging.max;
		return new CollectionPaging(data.slice(indexStart, indexEnd), (indexEnd > data.length) ? null : {page: paging.page + 1, max: paging.max});
	}
};
