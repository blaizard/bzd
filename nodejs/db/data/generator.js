const dropNewerOrEqual = (values, timestamp) => {
	const firstIndexToKeep = values.findIndex(([valueTimestamp]) => valueTimestamp < timestamp);
	values.splice(0, firstIndexToKeep == -1 ? values.length : firstIndexToKeep);
};

export default class Generator {
	constructor(data, uid, key, children, after, before) {
		this.data = data;
		this.uid = uid;
		this.key = key;
		this.children = children;
		this.after = after;
		this.before = before;
		this.columns = null;
		this.continuation = null;
	}

	async _fetchData(count = 1000) {
		const maybeData = await this.data.get({
			uid: this.uid,
			key: this.key,
			metadata: true,
			children: this.children,
			count: count,
			after: this.after,
			continuation: this.continuation,
		});
		if (maybeData.isEmpty()) {
			return null;
		}
		const value = maybeData.value();
		this.continuation = value.continuation ?? null;
		// Handle the case when only a single value is requested.
		const normalizedData = this.children == 0 ? [[[], value.data]] : value.data;
		// Convert the key into a string, remove the entries that are too new
		// and filter out the empty entries.
		const allValues = normalizedData
			.map(([key, values]) => {
				if (this.before !== null) {
					dropNewerOrEqual(values, this.before);
				}
				return [key.join("."), values];
			})
			.filter(([_key, values]) => values.length > 0);

		if (allValues.length == 0) {
			return null;
		}

		// This should be done only once on the first request.
		if (this.columns === null) {
			this.columns = allValues.map(([key]) => key);
			this.columns.sort();
		}

		return allValues;
	}

	async getColumns() {
		if (this.columns === null) {
			await this._fetchData(1);
			this.continuation = null;
		}
		return this.columns || [];
	}

	async *byTimestamp() {
		// Per-key state to consume the pages newest first.
		const nextValues = Object.create(null);
		const lastEmitted = Object.create(null);
		while (true) {
			const allValues = await this._fetchData();
			if (!allValues) {
				break;
			}
			// A key whose continuation is done is re-fetched from scratch on the
			// next page, drop the values that were already emitted.
			for (const [key, values] of allValues) {
				if (lastEmitted[key] !== undefined) {
					dropNewerOrEqual(values, lastEmitted[key]);
				}
				values.reverse();
			}

			while (true) {
				// Refill the next newest value for every key that can still provide one.
				for (const [key, values] of allValues) {
					if (nextValues[key] === undefined && values.length) {
						nextValues[key] = values.pop();
					}
				}
				if (Object.keys(nextValues).length === 0) {
					break;
				}

				// Only keep the values with the newest timestamp.
				const timestamp = Math.max(...Object.values(nextValues).map(([t]) => t));
				const yieldValuesPair = Object.entries(nextValues).filter(([, [t]]) => t == timestamp);
				for (const [key] of yieldValuesPair) {
					delete nextValues[key];
					lastEmitted[key] = timestamp;
				}

				yield [timestamp, Object.fromEntries(yieldValuesPair.map(([key, [, v]]) => [key, v]))];
			}

			// If there is no continuation, all the data has been fetched.
			if (this.continuation === null) {
				break;
			}
		}
	}
}
