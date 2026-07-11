import ValidationSchema from "#bzd/nodejs/core/validation_schema.js";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";

const Exception = ExceptionFactory("db", "data", "handlers", "validation");

export default class ValidationHandler {
	constructor(schema) {
		this.validation = new ValidationSchema(schema);
	}

	process(fragments) {
		let values = {};
		try {
			for (const fragment of fragments.all()) {
				const keys = fragment.key;
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

		try {
			this.validation.validate(values);
		} catch (e) {
			Exception.errorPrecondition("validation: {}", e.message);
		}
	}
}
