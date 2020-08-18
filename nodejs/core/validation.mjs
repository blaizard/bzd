import Format from "./format.mjs";
import ExceptionFactory from "./exception.mjs";

const Exception = ExceptionFactory("validation");

class Constraint {
	constructor(schema, key, name, args) {
		this.name = name;
		this.args = args;
		this.schema = schema;
		this.key = key;

		Exception.assert(
			this.isValidNbArgs(this.args.length),
			"Wrong number of arguments passed to '{}': {}",
			this.name,
			this.args.length
		);

		this.create();
	}

	get entry() {
		return this.schema[this.key];
	}

	create() {}

	install() {
		return (/*result, value*/) => {};
	}

	isValidNbArgs(/*n*/) {
		return true;
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
 *     confirm: "mandatory same(password)",
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
				value: schema
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
				constraints: []
			};

			if (typeof constraints == "string") {
				const contraintList = constraints.split(" ");
				contraintList
					.filter((constraint) => Boolean(constraint))
					.forEach((constraint) => {
						const m = constraint.match(patternConstraint);
						const name = m[1];
						const args = (m[2] || "").split(",").filter((arg) => arg !== "");
						this._processConstraint(processedSchema, key, name, args);
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
				value: values
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
				 * Callback deciding whether or not a valid is conisdered as existent
				 */
				valueExists: (key, value) => {
					return value !== undefined;
				},
				/**
				 * Ensure that all keys of values are present in the schema.
				 */
				all: false,
				/**
				 * This forces all values to be considered as mandatory.
				 */
				allMandatory: false
			},
			options
		);

		let result = {};

		for (const key in this.schema) {
			if (
				Constraint.assert(
					result,
					key,
					!(this.schema[key].mandatory || options.allMandatory) ||
						(key in values && options.valueExists(key, values[key])),
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

	_processConstraint(schema, key, name, args) {
		const singleValue = this.singleValue;
		const availableConstraints = {
			mandatory: class Mandatory extends Constraint {
				isValidNbArgs(n) {
					return n == 0;
				}
				create() {
					this.entry.mandatory = true;
				}
			},
			values: class Values extends Constraint {
				isValidNbArgs(n) {
					return n > 0;
				}
				install() {
					return (result, value) => {
						this.assert(result, this.args.includes(value), "must be equal to one of {:j}", this.args);
					};
				}
			},
			min: class Min extends Constraint {
				isValidNbArgs(n) {
					return n == 1;
				}
				install() {
					switch (this.entry.type) {
					case "string":
					case "integer":
						this.args[0] = Constraint.getAndAssertInteger(this.args[0]);
						break;
					case "float":
						this.args[0] = Constraint.getAndAssertFloat(this.args[0]);
					}

					switch (this.entry.type) {
					case "string":
						return (result, value) => {
							this.assert(result, value.length >= this.args[0], "at least {} characters", this.args[0]);
						};
					case "integer":
					case "float":
						return (result, value) => {
							this.assert(result, value >= this.args[0], "greater or equal to {}", this.args[0]);
						};
					default:
						Exception.unreachable("Unsupported format for 'min': {}", this.entry.type);
					}
				}
			},
			email: class Email extends Constraint {
				isValidNbArgs(n) {
					return n == 0;
				}
				install() {
					Exception.assert(this.entry.type == "string", "Email is only compatible with string type");
					return (result, value) => {
						this.assert(result, value.match(/^([a-zA-Z0-9_\-.]+)@([a-zA-Z0-9_\-.]+)\.([a-zA-Z]{2,5})$/), "email");
					};
				}
			},
			type: class Type extends Constraint {
				isValidNbArgs(n) {
					return n == 1;
				}
				create() {
					Exception.assert(
						["string", "integer", "float"].includes(this.args[0]),
						"'{}' is not a supported type",
						this.args[0]
					);
					this.entry.type = this.args[0];
				}
			},
			same: class Same extends Constraint {
				isValidNbArgs(n) {
					return n == 1;
				}
				install() {
					if (!singleValue) {
						Exception.assert(this.args[0] in this.schema, "Key '{}' is not set", this.args[0]);
					}
					return (result, value, values) => {
						if (singleValue) {
							return;
						}
						this.assert(result, value === values[this.args[0]], "different from {}", this.args[0]);
					};
				}
			}
		};

		Exception.assert(name in availableConstraints, "Unknown validation function '{}'", name);
		schema[key].constraints.push(new availableConstraints[name](schema, key, name, args));
	}
}
