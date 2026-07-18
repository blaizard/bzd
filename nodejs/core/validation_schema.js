import ExceptionFactory from "./exception.js";
import Result from "#bzd/nodejs/utils/result.js";
import { isObject, deepMerge } from "#bzd/nodejs/utils/object.js";

const Exception = ExceptionFactory("validation-schema");

export default class ValidationSchema {
	constructor(schema) {
		ValidationSchema.validateSchema(schema, {
			type: "object",
			properties: {
				type: {
					type: "string",
					enum: ["string", "number", "integer", "boolean", "null", "object", "array"],
				},
				minimum: {
					type: "number",
				},
				maximum: {
					type: "number",
				},
				unit: {
					type: "string",
				},
				description: {
					type: "string",
				},
				default: {},
				properties: {
					type: "object",
					additionalProperties: {
						$ref: "#",
					},
				},
				additionalProperties: {
					anyOf: [{ type: "boolean" }, { $ref: "#" }],
				},
				$ref: {
					type: "string",
				},
				$defs: {
					type: "object",
				},
				items: {
					$ref: "#",
				},
				required: {
					type: "array",
					items: {
						type: "string",
					},
				},
				enum: {
					type: "array",
					items: {
						type: "string",
					},
				},
				anyOf: {
					type: "array",
					items: {
						$ref: "#",
					},
				},
				allOf: {
					type: "array",
					items: {
						$ref: "#",
					},
				},
			},
			additionalProperties: false,
		});
		this.schema = schema;
	}

	/// Support only internal reference. This is a design choice to keep it simple and the code self contained.
	static getRefFromSchema(ref, schema) {
		Exception.assert(ref.startsWith("#"), "Only internal reference is supported: {}", ref);
		const path = ref
			.slice(1)
			.split("/")
			.filter((part) => part.length > 0);
		let current = schema;
		for (const part of path) {
			Exception.assert(part in current, "Invalid reference path: {}", ref);
			current = current[part];
		}
		return current;
	}

	static validationNumber(value, property, key) {
		if ("minimum" in property) {
			Exception.assert(
				value >= property.minimum,
				"[key={}] expected minimum value {}, not {:?}",
				key,
				property.minimum,
				value,
			);
		}
		if ("maximum" in property) {
			Exception.assert(
				value <= property.maximum,
				"[key={}] expected maximum value {}, not {:?}",
				key,
				property.maximum,
				value,
			);
		}
	}

	static validateSchema(values, schema, options = {}) {
		const validate = (valuesToValidate, schemaToValidate, nested) => {
			// Validate a single property, excluding anyOf, oneOf, allOf.
			const validateProperty = (value, property, key) => {
				try {
					if ("$ref" in property) {
						const propertyRef = ValidationSchema.getRefFromSchema(property["$ref"], schema);
						property = deepMerge({}, property, propertyRef);
					}

					switch (property.type) {
						case "string":
							Exception.assert(typeof value === "string", "[key={}] expected type string, not {:?}", key, value);
							if ("enum" in property) {
								Exception.assert(
									property.enum.includes(value),
									"[key={}] expected one of: {}, not {:?}",
									key,
									property.enum.join(", "),
									value,
								);
							}
							break;
						case "number":
							Exception.assert(typeof value === "number", "[key={}] expected type number, not {:?}", key, value);
							ValidationSchema.validationNumber(value, property, key);
							break;
						case "integer":
							Exception.assert(Number.isInteger(value), "[key={}] expected type integer, not {:?}", key, value);
							ValidationSchema.validationNumber(value, property, key);
							break;
						case "boolean":
							Exception.assert(typeof value === "boolean", "[key={}] expected type boolean, not {:?}", key, value);
							break;
						case "null":
							Exception.assert(value === null, "[key={}] expected 'null', not {:?}", key, value);
							break;
						case "object":
							Exception.assert(isObject(value), "[key={}] expected type object, not {:?}", key, value);
							const properties = property.properties || {};
							const additionalProperties = property.additionalProperties ?? true; // By default it is true.
							Object.entries(value).forEach(([subKey, subValue]) => {
								if (subKey in properties) {
									validate(subValue, properties[subKey], [...key, subKey]);
								} else if (isObject(additionalProperties)) {
									validate(subValue, additionalProperties, [...key, subKey]);
								}
							});
							if (property.required) {
								const required = new Set(property.required);
								const keys = new Set(Object.keys(value));
								Exception.assert(
									required.isSubsetOf(keys),
									"[key={}] Some required key(s) are not present: {}",
									key,
									[...required.difference(keys)].join(", "),
								);
							}
							if (additionalProperties === false) {
								const allowedKeys = new Set(Object.keys(properties));
								const keys = new Set(Object.keys(value));
								Exception.assert(
									keys.isSubsetOf(allowedKeys),
									"[key={}] Some key(s) are not allowed: {}",
									key,
									[...keys.difference(allowedKeys)].join(", "),
								);
							}
							break;
						case "array":
							Exception.assert(Array.isArray(value), "[key={}] expected type array, not {:?}", key, value);
							if ("items" in property) {
								value.forEach((item, index) => {
									validate(item, property.items, [...key, index]);
								});
							}
							break;
					}

					// Call the visitor.
					options.visitor && options.visitor(value, property, key);

					return null;
				} catch (e) {
					if (e instanceof Exception) {
						return e.message;
					}
					throw e;
				}
			};

			let maybeError = null;
			if ("allOf" in schemaToValidate) {
				schemaToValidate = deepMerge({}, schemaToValidate, ...schemaToValidate.allOf);
			}
			if ("anyOf" in schemaToValidate) {
				const results = schemaToValidate.anyOf.map((subSchema) =>
					validateProperty(valuesToValidate, deepMerge({}, schemaToValidate, subSchema), nested),
				);
				if (!results.some((result) => result === null)) {
					maybeError = "anyOf validation failed:\n" + results.join("\n");
				}
			} else {
				maybeError = validateProperty(valuesToValidate, schemaToValidate, nested);
			}
			if (maybeError) {
				throw new Exception(maybeError);
			}
		};

		validate(values, schema, []);
	}

	/// Options:
	/// visitor: (value, property, key) => { ... }
	validate(values, options = {}) {
		ValidationSchema.validateSchema(values, this.schema, options);
	}
}
