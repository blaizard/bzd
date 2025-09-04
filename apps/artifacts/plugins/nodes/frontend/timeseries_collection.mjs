import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes");
const Log = LogFactory("apps", "plugin", "nodes");

export default class TimeseriesCollection {
	constructor() {
		this.timeout = null;
		this.instanceCounter = 0;
		this.reset({});
	}

	/// Reset all collections.
	reset({ periodLimit = null }) {
		this.data = {};
		/// Maximum time span for the data.
		this.periodLimit = periodLimit;

		if (this.timeout !== null) {
			clearTimeout(this.timeout);
			this.timeout = null;
		}

		// Update the instance counter after each reset. Everything started under
		// an old instance counter will be dismissed.
		++this.instanceCounter;
	}

	close() {
		this.reset({});
	}

	_updateLimits(collection) {
		if (this.periodLimit === null) {
			return;
		}

		let data = this.data[collection] ?? [];
		if (data.length < 2) {
			return;
		}

		// Adjust the data based on the range, keep the last samples always.
		const timeLast = data.at(-1)[0];
		const currentRange = timeLast - data[0][0];
		if (currentRange > this.periodLimit) {
			const timeFirst = timeLast - this.periodLimit;
			const indexStart = data.findIndex((p) => p[0] >= timeFirst);
			data.splice(0, indexStart);
		}
	}

	/// Add a new chunk of data to the collection.
	////
	/// The data is expected to be an array of [time, value] pairs, sorted by time.
	/// If there is an overlap with existing data, the overlapping range is replaced.
	///
	/// \param data A dictionaty of arrays of [time, value] pairs, sorted by time.
	add(newData) {
		for (const [collection, collectionNewData] of Object.entries(newData)) {
			Exception.assertPrecondition(
				Array.isArray(collectionNewData),
				"Data must be an array, not {:j}",
				collectionNewData,
			);

			this.data[collection] ??= [];
			let data = this.data[collection];

			if (collectionNewData.length === 0) {
				return;
			}
			collectionNewData.sort((a, b) => a[0] - b[0]);

			const timeFirstNew = collectionNewData[0][0];
			const timeLastNew = collectionNewData.at(-1)[0];
			const indexStart = data.findIndex((p) => p[0] >= timeFirstNew);
			const indexEnd = data.findLastIndex((p) => p[0] <= timeLastNew);

			// Determine the actual range to replace.
			// If startIndex is -1, it means the new data is entirely after the old data,
			// but we already handled that. In an overlap scenario, it means we insert at the end.
			const deleteFrom = indexStart === -1 ? data.length : indexStart;

			// The number of items to delete. If nothing was found, this will be 0.
			const deleteCount = indexEnd === -1 || indexEnd < indexStart ? 0 : indexEnd - indexStart + 1;

			// Use splice to remove the old range and insert the new chunk.
			data.splice(deleteFrom, deleteCount, ...collectionNewData.map(([t, v, ..._]) => [t, v]));

			// Adjust the data based on the range, keep the last samples always.
			this._updateLimits(collection);

			// Sanity check.
			let previous = null;
			collectionNewData.forEach(([t, _], index) => {
				if (previous !== null && previous > t) {
					Log.warning(
						"Previous timestamp ({}) is greater than current ({}) at index {} on collection '{}'.",
						previous,
						t,
						index,
						collection,
					);
				}
				previous = t;
			});
		}
	}

	/// Calculate the time range of the data contained.
	///
	/// \return A tuple containing the oldest timestamp and the newest, or a tuple containing null and null.
	get timeRange() {
		let timestampNewest = null;
		let timestampOldest = null;
		for (const [_, data] of Object.entries(this.data)) {
			if (data.length > 0) {
				const currentTimestampNewest = data.at(-1)[0];
				const currentTimestampOldest = data.at(0)[0];
				timestampNewest =
					timestampNewest === null ? currentTimestampNewest : Math.max(timestampNewest, currentTimestampNewest);
				timestampOldest =
					timestampOldest === null ? currentTimestampOldest : Math.min(timestampOldest, currentTimestampOldest);
			}
		}
		return [timestampOldest, timestampNewest];
	}

	/// Register a function to refresh periodically the dataset.
	refreshPeriodically(callback, intervalS) {
		Exception.assert(this.timeout === null, "Periodic function already set.");
		const instanceCounter = this.instanceCounter;
		const timeoutCallback = async () => {
			let data = null;
			try {
				data = await callback(this.timeRange);
			} finally {
				if (instanceCounter == this.instanceCounter) {
					if (data) {
						this.add(data);
					}
					this.timeout = setTimeout(timeoutCallback, intervalS);
				}
			}
		};
		this.timeout = setTimeout(timeoutCallback, intervalS);
	}

	has(collection) {
		return collection in this.data;
	}

	get(collection) {
		return this.data[collection];
	}
}
