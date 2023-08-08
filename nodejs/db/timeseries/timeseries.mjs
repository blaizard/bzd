import {AsyncInitialize} from "../utils.mjs";

/**
 * File storage module
 */
export default class Storage extends AsyncInitialize {
  constructor() { super(); }

  /**
   * Insert a new entry
   */
  async insert(bucket, timestamp, data) { return this._insertImpl(bucket, timestamp, data); }

  /**
   * List the last N entries.
   */
  async list(bucket, maxOrPaging = 20) { return this._listImpl(bucket, maxOrPaging); }

  /**
   * List the last entries until a specific timestamp.
   */
  async listUntilTimestamp(bucket, timestamp, maxOrPaging = 20) {
	return this._listUntilTimestampImpl(bucket, timestamp, maxOrPaging);
  }
}
