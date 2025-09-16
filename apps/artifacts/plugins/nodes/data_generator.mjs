export default class DataGenerator {
	constructor(node, key, children, after, before) {
		this.node = node;
		this.key = key;
		this.children = children;
		this.after = after;
		this.before = before;
		this.columns = null;
	}

	async _fetchDataAfter(after, count = 1000) {
		if (this.before !== null && after > this.before) {
			return null;
		}

		const maybeData = await this.node.get({
			key: this.key,
			metadata: true,
			children: this.children,
			count: count,
			after: after,
		});
		if (maybeData.isEmpty()) {
			return null;
		}
		// Handle the case when only a single value is requested.
		const normalizedData = this.children == 0 ? [[[], maybeData.value()]] : maybeData.value();
		// This does the following:
		// - Convert the key into a string.
		// - Remove entries that are too new.
		// - Filter out the empty entries.
		const allValues = normalizedData
			.map(([key, values]) => {
				if (this.before !== null) {
					const firstIndexToKeep = values.findIndex(([timestamp]) => timestamp < this.before);
					if (firstIndexToKeep == -1) {
						values = [];
					} else {
						values.splice(0, firstIndexToKeep);
					}
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
			await this._fetchDataAfter(this.after, 1);
		}
		return this.columns || [];
	}

	async *byTimestamp() {
		let after = this.after;
		while (true) {
			const allValues = await this._fetchDataAfter(after);
			if (!allValues) {
				break;
			}

			let nextValues = {};
			const nbValues = allValues.length;
			while (true) {
				// Get the next oldest values for every keys and ensure ALL keys have been
				// populated, otherwise re-fetch.
				const nbCurrentValues = allValues.reduce((accmulator, [key, values]) => {
					nextValues[key] ??= values.pop();
					return accmulator + (nextValues[key] === undefined ? 0 : 1);
				}, 0);
				if (nbCurrentValues == 0 || nbCurrentValues != nbValues) {
					break;
				}

				// Only keep the values with the oldest timestamp.
				const timestamp = Math.min(...Object.values(nextValues).map(([t]) => t));
				const yieldValuesPair = Object.entries(nextValues)
					.map(([key, [t, v]]) => (t == timestamp ? [key, v] : false))
					.filter(Boolean);
				yieldValuesPair.forEach(([key, _]) => delete nextValues[key]);

				yield [timestamp, Object.fromEntries(yieldValuesPair)];

				after = timestamp;
			}
		}
	}
}
