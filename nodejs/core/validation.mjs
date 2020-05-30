"use strict";

import ExceptionFactory from "./exception.mjs";

const Exception = ExceptionFactory("validation");

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
		this.schema = this.processSchema(schema);
	}

	processSchema(schema) {

		const patternConstraint = new RegExp("([a-zA-Z0-9_-]+)(?:\\((.*)\\))?");

		let processedSchema = {};

		for (const key in schema) {
			const constraints = schema[key];
			processedSchema[key] = {
				mandatory: false,
				validations: []
			};

			if (typeof constraints == "string") {
				const contraintList = constraints.split(" ");
				contraintList.forEach((constraint) => {
					const m = constraint.match(patternConstraint);
					const name = m[1];
					const arg = m[2];
					this.processFunction(processedSchema[key], name, arg);
				});
			}
			else {
				Exception.unreachable("Constraints type is unsupported: {:j}", constraints);
			}
		}

		console.log(processedSchema);
	}

	processFunction(entry, name, arg) {
		const availableFunctions = {
			mandatory: (entry) => {
				entry.mandatory = true;
			},
			min: (entry, arg) => {
				Exception.assert(typeof arg != "undefined");
			}
		}

		Exception.assert(name in availableFunctions, "Unknown validation function '{}'", name);
		availableFunctions[name](entry, arg);
	}
}
