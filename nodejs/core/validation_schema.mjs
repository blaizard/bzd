import ExceptionFactory from "./exception.mjs";
import Result from "#bzd/nodejs/utils/result.mjs";
import { isObject } from "#bzd/nodejs/utils/object.mjs";

const Exception = ExceptionFactory("validation-schema");

export default class ValidationSchema {
	constructor(schema) {
		Exception.assert(isObject(schema), "Schema must be an object: {:j}", schema);
		this.schema = schema;
	}

	validate(values, options) {
		const validate = (values, schema, nested) => {
			const validateProperty = (value, property, key) => {
				switch (property.type) {
					case "string":
						Exception.assert(typeof value === "string", "[key={}] expected type string, not {:j}", key, value);
						if ("enum" in property) {
							Exception.assert(
								property.enum.includes(value),
								"[key={}] expected on of: {}, not {:j}",
								key,
								property.enum.join(", "),
								value,
							);
						}
						break;
					case "number":
						Exception.assert(typeof value === "number", "[key={}] expected type number, not {:j}", key, value);
						break;
					case "integer":
						Exception.assert(Number.isInteger(value), "[key={}] expected type integer, not {:j}", key, value);
						break;
					case "boolean":
						Exception.assert(typeof value === "boolean", "[key={}] expected type boolean, not {:j}", key, value);
						break;
					case "null":
						Exception.assert(value === null, "[key={}] expected 'null', not {:j}", key, value);
						break;
					case "object":
						Exception.assert(isObject(value), "[key={}] expected type object, not {:j}", key, value);
						if ("properties" in property) {
							validate(value, property, key);
						}
						break;
					case "array":
						Exception.assert(Array.isArray(value), "[key={}] expected type array, not {:j}", key, value);
						if ("items" in property) {
							value.forEach((item, index) => {
								validateProperty(item, property.items, key + "[" + index + "]");
							});
						}
						break;
					default:
						Exception.unreachable("[key={}] schema type '{}' unsupported.", key, property.type);
				}
			};

			Exception.assert(isObject(values), "Input values must be an object: {:j}", values);
			const properties = schema.properties || {};
			for (const [key, value] of Object.entries(values)) {
				if (key in properties) {
					validateProperty(value, properties[key], nested + "." + key);
				}
			}
			if (schema.required) {
				const required = new Set(schema["required"]);
				const keys = new Set(Object.keys(values));
				Exception.assert(
					required.isSubsetOf(keys),
					"[key={}] Some required key(s) are not present: {}",
					nested,
					[...required.difference(keys)].join(", "),
				);
			}
			const additionalProperties = schema.additionalProperties ?? true;
			// By default it is true.
			if (!additionalProperties) {
				const allowedKeys = new Set(Object.keys(properties));
				const keys = new Set(Object.keys(values));
				Exception.assert(
					keys.isSubsetOf(allowedKeys),
					"[key={}] Some key(s) are not allowed: {}",
					nested,
					[...keys.difference(allowedKeys)].join(", "),
				);
			}
		};

		validate(values, this.schema, "");
	}
}
