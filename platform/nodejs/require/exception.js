"use strict";

const Log = require("./log.js")("exception");

const ExceptionFactory = (...topics) => {
	return class Exception extends Error {

		constructor(...args) {

			// This should capture a callstack
			super();

			// Nested exceptions (previous exception that have been merged with this one)
			this.nestedErrorList = [];
			// Topics associated with this object
			this.topics = topics;
			args.forEach((arg) => {
				// Merge nested exceptions
				if (arg instanceof Error) {
					this.nestedErrorList.push((arg instanceof Exception) ? arg : Exception.fromError(arg));
					this.nestedErrorList = this.nestedErrorList.concat(arg.nestedErrorList || []);
				}
				else {
					this.message += ((this.message) ? "; " : "") + String(arg);
				}
			});
		}

		/**
		 * Convert an Error into an Exception
		 */
		static fromError(e)
		{
			let exception = new Exception();
			exception.message = e.message;
			exception.stack = e.stack;
			if (e.hasOwnProperty("topics")) {
				exception.topics = e.topics;
			}
			if (e.hasOwnProperty("nestedErrorList")) {
				exception.nestedErrorList = e.nestedErrorList;
			}
			return exception;
		}

		/**
		 * \brief Assert that expression evaluates to true.
		 *
		 * \param expression The expression to evaluate.
		 * \param ...args (optional) The message to display if the assertion fails.
		 */
		static assert(expression, ...args) {
			if (!expression) {
				args.unshift("Assertion failed");
				throw new Exception(...args.map((arg) => ((typeof arg === "function") ? arg() : arg)));
			}
		}

		/**
		 * \brief Assert that 2 values are equal.
		 * This is not a strict assert.
		 *
		 * \param value1 The first value.
		 * \param value2 The second value.
		 * \param ...args (optional) The message to display if the assertion fails.
		 */
		static assertEqual(value1, value2, ...args) {

			const assertEqualInternal = (value1, value2, ...args) => {
				if (typeof value1 === "object" && value1 !== null && typeof value2 === "object" && value2 !== null) {
					if (value1 instanceof Array && value2 instanceof Array) {
						Exception.assert(value1.length === value2.length, ...args);
						value1.forEach((subValue1, index) => { assertEqualInternal(subValue1, value2[index], ...args); });
					}
					else {
						assertEqualInternal(Object.keys(value1), Object.keys(value2), ...args);
						Object.keys(value1).forEach((key) => { assertEqualInternal(value1[key], value2[key], ...args); });
					}
				}
				else {
					Exception.assert(value1 == value2, ...args);
				}
			}

			assertEqualInternal(value1, value2, () => ("Values are not equal, value1=" + JSON.stringify(value1) + ", value2=" + JSON.stringify(value2)), ...args);
		}

		/**
		 * Ensures that a specific block of code throws an exception
		 */
		static async assertThrows(block, ...args) {
			let hasThrown = false;

			try {
				await block();
			}
			catch (e) {
				hasThrown = true;
			}
			finally {
				Exception.assert(hasThrown, "Code block did not throw", ...args);
			}
		}

		/**
		 * Flag a line of code unreachable
		 */
		static unreachable(...args) {
			throw new Exception("Code unreachable", ...args);
		}

		/**
		 * \brief Print a formated exception message
		 */
		static print(...args) {
			Log.custom({
				level: "error",
				topics: topics
			}, ...args);
		}

		/**
		 * \brief Print a formated exception message
		 */
		print(...args) {
			if (args) {
				Exception.print(...args);
			}
			Log.custom({level: "error"}, this);
		}

		/**
		 * \brief Print the current exception object
		 */
		toString() {

			let message = "[" + this.topics.join("::") + "] " + this.name
					+ ((this.message) ? (" with message: " + String(this.message)) : "")
					// Remove the first line of the stack to avoid poluting the output
					+ ((this.stack) ? ("; Callstack:\n" + String(this.stack).split("\n").slice(1).join("\n")) : "");
			message += this.nestedErrorList.map((e) => ("\nFrom: " + String(e)));
			return message;
		}
	};
};

const E = ExceptionFactory("exception");

// Register uncaught exception handler
process.on("uncaughtException", (e) => {
	E.fromError(e).print("Exception: uncaughtException");
});
process.on("unhandledRejection", (e) => {
	E.fromError(e).print("Exception: unhandledRejection");
});
if (typeof window !== "undefined") {
	window.addEventListener("error", (e) => {
		E.fromError(e).print();
		return false;
	});
}

module.exports = ExceptionFactory;
