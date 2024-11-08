import { AsyncInitialize } from "../utils.mjs";
import ExceptionFactory from "../../core/exception.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";

const Exception = ExceptionFactory("db", "kvs");

class KeyValueStoreBucketAccessor {
	constructor(storage, bucket) {
		this.storage = storage;
		this.bucket = bucket;
	}

	/// Set a key/value pair.
	///
	/// If key is set to null, a unique key is automatically generated.
	async set(key, value) {
		return this.storage._setImpl(this.bucket, key, value);
	}

	/// Retrieve a value from a given key, or return the default value if it does not exists.
	async get(key, defaultValue = undefined) {
		return this.storage._getImpl(this.bucket, key, defaultValue);
	}

	/// Atomic operation that consists of a read, modify and write operation.
	async update(key, modifier = async (value) => value, defaultValue = undefined, maxConflicts = 100) {
		return this.storage._updateImpl(this.bucket, key, modifier, defaultValue, maxConflicts);
	}

	/// Check if a key exists.
	async is(key) {
		return this.storage.is(this.bucket, key);
	}

	/// Return the number of entries in this bucket.
	async count() {
		return this.storage._countImpl(this.bucket);
	}

	/// List all key/value pairs from this bucket.
	/// \param maxOrPaging Paging information.
	async list(maxOrPaging = 20) {
		return this.storage._listImpl(this.bucket, maxOrPaging);
	}

	/// List all key/value pairs from this bucket which subkey matches the value (or any of the values).
	/// \param subKey The subkey for the match.
	/// \param value The value of values (if a list) to match.
	/// \param maxOrPaging Paging information.
	/// \return An object containing the data and the information about paging and how to get the rest of the data.
	async listMatch(subKey, value, maxOrPaging = 20) {
		return this.storage._listMatchImpl(this.bucket, subKey, value, maxOrPaging);
	}

	/// Delete an existing key/value pair.
	async delete(key) {
		return this.storage._deleteImpl(this.bucket, key);
	}
}

/// Key value store for low demanding application, that persists on the local disk.
export default class KeyValueStore extends AsyncInitialize {
	constructor() {
		super();
	}

	/// Set a key/value pair.
	///
	/// If key is set to null, a unique key is automatically generated.
	async set(bucket, key, value) {
		return this._setImpl(bucket, key, value);
	}

	/// Retrieve a value from a given key, or return the default value if it does not exists.
	async get(bucket, key, defaultValue = undefined) {
		return this._getImpl(bucket, key, defaultValue);
	}

	/// Atomic operation that consists of a read, modify and write operation.
	async update(bucket, key, modifier = async (value) => value, defaultValue = undefined, maxConflicts = 100) {
		return this._updateImpl(bucket, key, modifier, defaultValue, maxConflicts);
	}

	/// Check if a key exists.
	async is(bucket, key) {
		return (await this.get(bucket, key)) !== undefined;
	}

	/// Return the number of entries in this bucket.
	async count(bucket) {
		return this._countImpl(bucket);
	}

	/// List all key/value pairs from this bucket.
	/// \param bucket The bucket to be used.
	/// \param maxOrPaging Paging information.
	async list(bucket, maxOrPaging = 20) {
		return this._listImpl(bucket, maxOrPaging);
	}

	/// List all key/value pairs from this bucket which subkey matches the value (or any of the values).
	/// \param bucket The bucket to be used.
	/// \param subKey The subkey for the match.
	/// \param value The value of values (if a list) to match.
	/// \param maxOrPaging Paging information.
	/// \return An object containing the data and the information about paging and how to get the rest of the data.
	async listMatch(bucket, subKey, value, maxOrPaging = 20) {
		return this._listMatchImpl(bucket, subKey, value, maxOrPaging);
	}

	/// Delete an existing key/value pair.
	async delete(bucket, key) {
		return this._deleteImpl(bucket, key);
	}

	/// Create a dictionary of all the data from a bucket.
	async dump(bucket) {
		let result = {};
		for await (const [key, value] of CollectionPaging.makeIterator(async (maxOrPaging) => {
			return await this.list(bucket, maxOrPaging);
		}, 50)) {
			result[key] = value;
		}
		return result;
	}

	/// Create an accessor for a specific bucket.
	/// \param bucket The bucket to be used.
	getAccessor(bucket) {
		return new KeyValueStoreBucketAccessor(this, bucket);
	}

	/// Run a suite of validation tests.
	async runValidationTests(bucket = "validation_tests") {
		try {
			// Create several entries.
			await this.set(bucket, "entry1", { a: 76, c: { d: 12 } });
			await this.set(bucket, "entry2", 12);

			// Read the entries.
			Exception.assertEqual(await this.get(bucket, "entry1"), { a: 76, c: { d: 12 } });
			Exception.assertEqual(await this.get(bucket, "entry2"), 12);

			// Override entries.
			await this.set(bucket, "entry2", { a: 42 });
			Exception.assertEqual(await this.get(bucket, "entry2"), { a: 42 });

			// Read unexisting entry.
			Exception.assertEqual(await this.get(bucket, "entry3", "not present"), "not present");
			Exception.assert(!(await this.is(bucket, "entry3")));
			Exception.assert(await this.is(bucket, "entry1"));

			// Check count
			Exception.assertEqual(await this.count(bucket), 2);
			Exception.assertEqual(await this.count("non-existing-bucket-for-testing"), 0);

			// Update existing.
			await this.update(bucket, "entry1", (value) => {
				++value.c.d;
				return value;
			});
			Exception.assertEqual(await this.get(bucket, "entry1"), { a: 76, c: { d: 13 } });

			// Update new entry.
			await this.update(
				bucket,
				"entry3",
				(value) => {
					return value;
				},
				{ c: -23 },
			);
			Exception.assertEqual(await this.get(bucket, "entry3"), { c: -23 });

			// Delete.
			Exception.assertEqual(await this.count(bucket), 3);
			await this.delete(bucket, "entry3");
			Exception.assertEqual(await this.count(bucket), 2);

			// Delete non existing.
			await this.delete(bucket, "entry4");

			// List.
			{
				const result = await this.list(bucket);
				Exception.assert(result.isLast());
				Exception.assertEqual(result.data(), { entry1: { a: 76, c: { d: 13 } }, entry2: { a: 42 } });
			}
			// List with pagination.
			{
				const result1 = await this.list(bucket, 1);
				Exception.assert(!result1.isLast());
				Exception.assertEqual(result1.data(), { entry1: { a: 76, c: { d: 13 } } });
				const result2 = await this.list(bucket, result1.getNextPaging());
				Exception.assert(result2.isLast());
				Exception.assertEqual(result2.data(), { entry2: { a: 42 } });
			}

			// ListMatch
			{
				const result = await this.listMatch(bucket, "a", 42);
				Exception.assert(result.isLast());
				Exception.assertEqual(result.data(), { entry2: { a: 42 } });
			}
			{
				const result = await this.listMatch(bucket, "a", 76);
				Exception.assert(result.isLast());
				Exception.assertEqual(result.data(), { entry1: { a: 76, c: { d: 13 } } });
			}
			{
				const result = await this.listMatch(bucket, "a", 4343);
				Exception.assert(result.isLast());
				Exception.assertEqual(result.data(), {});
			}
		} finally {
			await this.delete(bucket, "entry1");
			await this.delete(bucket, "entry2");
			await this.delete(bucket, "entry3");

			Exception.assertEqual(await this.count(bucket), 0);

			// List on an empty bucket.
			{
				const result = await this.list(bucket);
				Exception.assert(result.isLast());
				Exception.assertEqual(result.data(), {});
			}
		}
	}
}
