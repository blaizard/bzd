import format from "#bzd/nodejs/core/format.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import KeyMapping from "#bzd/apps/artifacts/plugins/nodes/key_mapping.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes", "handlers", "to_string");

export default class ToStringHandler {
	constructor(pattern) {
		this.pattern = pattern;
	}

	process(fragments) {
		// Extract all fragments that are 1-level deep and create a dictionary of their values.
		const values = {};
		for (const fragment of fragments.all()) {
			const key = fragment.key;
			if (key.length >= 1) {
				const last = key.pop();
				const entryKey = KeyMapping.keyToInternal(key);
				values[entryKey] ??= {
					values: {},
					options: {},
				};
				values[entryKey].values[last] = fragment.value;
				Object.assign(values[entryKey].options, fragment.options);
				fragment.remove();
			}
		}

		for (const [internal, data] of Object.entries(values)) {
			try {
				fragments.add(KeyMapping.internalToKey(internal), format(this.pattern, data.values), data.options);
			} catch (e) {
				Exception.errorPrecondition("toString for {}: {}", KeyMapping.internalToPath(internal), e.message);
			}
		}
	}
}
