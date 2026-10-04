import KeyMapping from "#bzd/nodejs/db/data/key_mapping.js";

/// Re-nest the flattened fragments back into dictionaries.
///
/// The storage layer flattens any dictionary value into one leaf per key.
/// This handler is the inverse: fragments sharing the same prefix are grouped
/// back into a single dictionary value, e.g. [name, topic] -> [name] -> {topic: value}.
export default class GroupHandler {
	process(fragments) {
		const values = Object.create(null);
		for (const fragment of fragments.all()) {
			const key = fragment.key;
			if (key.length < 2) {
				continue;
			}
			const last = key.pop();
			const entryKey = KeyMapping.keyToInternal(key);
			values[entryKey] ??= {
				values: Object.create(null),
				options: {},
			};
			values[entryKey].values[last] = fragment.value;
			Object.assign(values[entryKey].options, fragment.options);
			fragment.remove();
		}

		for (const [internal, data] of Object.entries(values)) {
			fragments.add(KeyMapping.internalToKey(internal), data.values, data.options);
		}
	}
}
