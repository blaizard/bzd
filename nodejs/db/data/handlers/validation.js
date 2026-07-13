import ValidationSchema from "#bzd/nodejs/core/validation_schema.js";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import KeyMapping from "#bzd/nodejs/db/data/key_mapping.js";

const Exception = ExceptionFactory("db", "data", "handlers", "validation");

export default class ValidationHandler {
	constructor(schema) {
		this.validation = new ValidationSchema(schema);
		this.processed = new Set();
	}

	process(fragments) {
		let values = {};
		let fragmentsToProcess = {};
		try {
			for (const fragment of fragments.all()) {
				const keys = fragment.key;
				const hash = KeyMapping.keyToInternal(keys);
				if (!this.processed.has(hash)) {
					fragmentsToProcess[hash] = fragment;
					this.processed.add(hash);
				}
				if (keys.length === 0) {
					values = fragment.value;
				} else {
					let current = values;
					const lastIndex = keys.length - 1;
					for (let i = 0; i < lastIndex; i++) {
						current[keys[i]] ??= {};
						current = current[keys[i]];
					}
					current[keys[lastIndex]] = fragment.value;
				}
			}
		} catch (e) {
			Exception.errorPrecondition("validation: values are not json-compatible, {}", e.message);
		}

		let options = {};
		if (Object.keys(fragmentsToProcess).length > 0) {
			options.visitor = (value, property, key) => {
				const hash = KeyMapping.keyToInternal(key);
				if (hash in fragmentsToProcess) {
					const unit = property.type == "array" ? property?.items?.unit : property?.unit;
					if (unit) {
						fragmentsToProcess[hash].options.unit = unit;
					}
				}
			};
		}

		try {
			this.validation.validate(values, options);
		} catch (e) {
			Exception.errorPrecondition("validation: {}", e.message);
		}
	}
}
