import Format from "./format.mjs";
import ExceptionFactory from "./exception.mjs";

const Exception = ExceptionFactory("validation");

class Constraint {
	constructor(schema, key, name, arg) {
		this.name = name;
		this.arg = arg;
		this.schema = schema;
		this.key = key;

		this.create();
	}

	get entry() {
		return this.schema[this.key];
	}

	create() {}

	install() {
		return (/*result, value*/) => {};
	}

	static getAndAssertInteger(arg, result = [], key = null) {
		const num = parseInt(arg);
		if (Constraint.assert(result, key, !isNaN(num) && String(arg).match(/^-?[0-9]+$/), "integer")) {
			return num;
		}
		Exception.assert(key !== null, "Argument is not a valid integer: '{}'", arg);
		return undefined;
	}

	static getAndAssertFloat(arg, result = [], key = null) {
		const num = parseFloat(arg);
		if (Constraint.assert(result, key, !isNaN(num) && String(arg).match(/^-?[0-9]+.?[0-9]*$/), "float")) {
			return num;
		}
		Exception.assert(key !== null, "Argument is not a valid integer: '{}'", arg);
		return undefined;
	}

	static assert(result, key, condition, str, ...args) {
		if (!condition) {
			if (!(key in result)) {
				result[key] = [];
			}
			result[key].push(Format(str, ...args));
			return false;
		}
		return true;
	}

	assert(result, condition, str, ...args) {
		return Constraint.assert(result, this.key, condition, str, ...args);
	}
}

/**
 * Data validation module
 *
 * Validation definition, can be doen through json.
 *
 * Here is an example:
 * {
 *     email: "mandatory email",
 *     password: "mandatory min(6)",
 *     confirm: "mandatory equal(password)",
 *     stayConnnected: "type(bool)"
 * }
 *
 * To each keys is associated a constraint. A constraint definition can be done
 * from a string. Each contraints are space separated and must follow the same format:
 * <name>(<argument>)?
 * If the optional argument contains a space, the argument must have brakets.
 * By default all keys are considered as optional unless mandatory is specified.
 */
export default class Validation {
	constructor(schema, singleValue = false) {
		if (singleValue) {
			schema = {
				value: schema,
			};
		}
		this.singleValue = singleValue;
		this.schema = this._processSchema(schema);
	}

	_processSchema(schema) {
		const patternConstraint = new RegExp("([a-zA-Z0-9_-]+)(?:\\((.*)\\))?");

		let processedSchema = {};

		for (const key in schema) {
			const constraints = schema[key];
			processedSchema[key] = {
				mandatory: false,
				type: "string",
				constraints: [],
			};

			if (typeof constraints == "string") {
				const contraintList = constraints.split(" ");
				contraintList
					.filter((constraint) => Boolean(constraint))
					.forEach((constraint) => {
						const m = constraint.match(patternConstraint);
						const name = m[1];
						const arg = m[2];
						this._processConstraint(processedSchema, key, name, arg);
					});
			}
			else {
				Exception.unreachable("Constraints type is unsupported: {:j}", constraints);
			}
		}

		// Install all constraints
		for (const key in processedSchema) {
			processedSchema[key].constraints = processedSchema[key].constraints.map((constraint) => constraint.install());
		}

		return processedSchema;
	}

	/**
	 * Validate the specific value passed into argument.
	 * Value must be a dictionary.
	 */
	validate(values, options) {
		// Convert the value into a dictionary
		if (this.singleValue) {
			values = {
				value: values,
			};
		}
		Exception.assert(typeof values == "object", "Value must be a dictionary: {:j}", values);

		options = Object.assign(
			{
				/**
				 * Output type, can be either "throw" or "return"
				 */
				output: "throw",
				/**
				 * Callback decifing whether or not a valid is conisdered as existent
				 */
				valueExists: (key, value) => {
					return value !== undefined;
				},
				/**
				 * Ensure that all keys of values are present in the schema.
				 */
				all: false,
			},
			options
		);

		let result = {};

		for (const key in this.schema) {
			if (
				Constraint.assert(
					result,
					key,
					!this.schema[key].mandatory || (key in values && options.valueExists(key, values[key])),
					"mandatory"
				)
			) {
				if (key in values) {
					let value = values[key];
					switch (this.schema[key].type) {
					case "integer":
						value = Constraint.getAndAssertInteger(value, result, key);
						break;
					case "float":
						value = Constraint.getAndAssertFloat(value, result, key);
						break;
					}
					if (value !== undefined) {
						this.schema[key].constraints.forEach((constraint) => constraint(result, value, values));
					}
				}
			}
		}

		// Check that all keys have been covered
		if (options.all) {
			for (const key in values) {
				Constraint.assert(result, key, key in this.schema, "Key '{}' is missing", key);
			}
		}

		switch (options.output) {
		case "throw":
			{
				const keys = Object.keys(result);
				if (keys.length == 1) {
					throw new Exception("'{}' does not validate: {}", keys[0], result[keys[0]].join(", "));
				}
				else if (keys.length > 1) {
					const message = keys.map((key) => {
						return String(key) + ": (" + result[key].join(", ") + ")";
					});
					throw new Exception("Some values do not validate: {}", message.join("; "));
				}
			}
			break;
		case "return":
			return result;
		default:
			Exception.unreachable("Unsupported output type: '{}'", options.output);
		}
	}

	isMandatory(name) {
		return name in this.schema ? this.schema[name].mandatory : false;
	}

	_processConstraint(schema, key, name, arg) {
		const singleValue = this.singleValue;
		const availableConstraints = {
			mandatory: class Mandatory extends Constraint {
				create() {
					this.entry.mandatory = true;
				}
			},
			min: class Min extends Constraint {
				install() {
					switch (this.entry.type) {
					case "string":
					case "integer":
						this.arg = Constraint.getAndAssertInteger(this.arg);
						break;
					case "float":
						this.arg = Constraint.getAndAssertFloat(this.arg);
					}

					switch (this.entry.type) {
					case "string":
						return (result, value) => {
							this.assert(result, value.length >= this.arg, "at least {} characters", this.arg);
						};
					case "integer":
					case "float":
						return (result, value) => {
							this.assert(result, value >= this.arg, "greater or equal to {}", this.arg);
						};
					default:
						Exception.unreachable("Unsupported format for 'min': {}", this.entry.type);
					}
				}
			},
			email: class Email extends Constraint {
				install() {
					Exception.assert(this.entry.type == "string", "Email is only compatible with string type");
					return (result, value) => {
						this.assert(result, value.match(/^([a-zA-Z0-9_\-.]+)@([a-zA-Z0-9_\-.]+)\.([a-zA-Z]{2,5})$/), "email");
					};
				}
			},
			type: class Type extends Constraint {
				create() {
					Exception.assert(["string", "integer", "float"].includes(this.arg), "'{}' is not a supported type", this.arg);
					this.entry.type = this.arg;
				}
			},
			same: class Same extends Constraint {
				install() {
					if (!singleValue) {
						Exception.assert(this.arg in this.schema, "Key '{}' is not set", this.arg);
					}
					return (result, value, values) => {
						if (singleValue) {
							return;
						}
						this.assert(result, value === values[this.arg], "different from {}", this.arg);
					};
				}
			},
		};

		Exception.assert(name in availableConstraints, "Unknown validation function '{}'", name);
		schema[key].constraints.push(new availableConstraints[name](schema, key, name, arg));
	}
}
