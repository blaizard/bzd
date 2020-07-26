import ExceptionFactory from "../../core/exception.mjs";
import { AsyncInitialize } from "../utils.mjs";

const Exception = ExceptionFactory("db", "timeseries");

/**
 * File storage module
 */
export default class Storage extends AsyncInitialize {
	constructor() {
		super();
	}

	/**
	 * Insert a new entry
	 */
	async insert(/*bucket, timestamp, data*/) {
		Exception.unreachable("'insert' must be implemented.");
	}

	/**
	 * List the last N entries.
	 */
	async list(/*bucket, maxOrPaging = 10*/) {
		Exception.unreachable("'list' must be implemented.");
	}

	/**
	 * List the last entries until a specific timestamp.
	 */
	async listUntilTimestamp(/*bucket, timestamp, maxOrPaging = 10*/) {
		Exception.unreachable("'listUntilTimestamp' must be implemented.");
	}
}
