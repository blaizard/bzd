"use strict";

import ExceptionFactory from "./exception.mjs";

const Exception = ExceptionFactory("validation");

class Constraint
{
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

	create() {
	}

	install() {
		return (/*value*/) => {}
	}

	static assertInteger(arg) {
		const num = parseInt(arg);
		Exception.assert(!isNaN(num) && String(arg).match(/^-?[0-9]+$/), "Argument must be a valid integer: '{}'", arg);
		return num;
	}

	static assertFloat(arg) {
		const num = parseFloat(arg);
		Exception.assert(!isNaN(num) && String(arg).match(/^-?[0-9]+.?[0-9]*$/), "Argument must be a valid float: '{}'", arg);
		return num;
	}
};

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
	constructor(schema) {
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
				contraintList.forEach((constraint) => {
					const m = constraint.match(patternConstraint);
					const name = m[1];
					const arg = m[2];
					this.processConstraint(processedSchema, key, name, arg);
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
	validate(values) {
		Exception.assert(typeof values == "object", "Value must be a dictionary: {:j}", values);

		for (const key in this.schema) {
			Exception.assert(!this.schema[key].mandatory || key in values, "Missing mandatory key '{}'", key);
			if (key in values) {
				let value = values[key]
				switch (this.schema[key].type) {
				case "integer":
					value = Constraint.assertInteger(value);
					break;
				case "float":
					value = Constraint.assertFloat(value);
					break;
				}
				this.schema[key].constraints.forEach((constraint) => constraint(value));
			}
		}
	}

	processConstraint(schema, key, name, arg) {
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
						this.arg = Constraint.assertInteger(this.arg);
						break;
					case "float":
						this.arg = Constraint.assertFloat(this.arg);
					}

					switch (this.entry.type) {
					case "string":
						return (value) => {
							Exception.assert(value.length >= this.arg, "Length must be >= {}", this.arg);
						};
					case "integer":
					case "float":
						return (value) => {
							Exception.assert(value >= this.arg, "Number must be >= {}", this.arg);
						};
					default:
						Exception.unreachable("Unsupported format for 'min': {}", this.entry.type);
					}
				}
			},
			type: class Type extends Constraint {
				create() {
					Exception.assert(["string", "integer", "float"].includes(this.arg), "'{}' is not a supported type", this.arg);
					this.entry.type = this.arg;
				}
			},
		};

		Exception.assert(name in availableConstraints, "Unknown validation function '{}'", name);
		schema[key].constraints.push(new availableConstraints[name](schema, key, name, arg));
	}
}
