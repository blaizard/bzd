import Format from "./format.mjs";
import LogFactory from "./log.mjs";
import uncaughtExceptionHandler from "./impl/exception/backend.mjs";
const Log = LogFactory("exception");

export class ExceptionPrecondition extends Error {
	constructor(exception) {
		super();
		this.exception = exception;
	}

	get message() {
		return this.exception.message;
	}

	toString() {
		return String(this.exception);
	}
}

/**
 * Private class to combine exception elements
 */
class ExceptionCombine {
	constructor(...args) {
		this.list = [args];
	}
	add(str, ...args) {
		if (str instanceof ExceptionCombine) {
			this.list = this.list.concat(str.list);
		} else {
			this.list.push([str, ...args]);
		}
	}
	[Symbol.iterator]() {
		return this.list.values();
	}
}

export const ExceptionFactory = (...topics) => {
	return class Exception extends Error {
		constructor(str = "", ...args) {
			// This should capture a callstack
			super();

			// Nested exceptions (previous exception that have been merged with this one)
			this.nestedErrorList = [];
			// Topics associated with this object
			this.topics = topics;

			// Set the message
			if (str instanceof ExceptionCombine) {
				let messageList = [];
				for (const a of str) {
					messageList.push(this._init(a[0], ...a.slice(1)));
				}
				this.message = messageList.filter((item) => item).join("; ");
			} else {
				this.message = this._init(str, ...args);
			}
		}

		_init(str, ...args) {
			if (typeof str == "string") {
				return Format(str, ...args);
			}
			if (typeof str == "function") {
				return str();
			}
			if (str instanceof Error) {
				this.nestedErrorList.push(str instanceof Exception ? str : Exception.fromError(str));
				this.nestedErrorList = this.nestedErrorList.concat(str.nestedErrorList || []);
				return "";
			}
			return String(str);
		}

		static makePreconditionException(str = "", ...args) {
			const exception = new Exception(str, ...args);
			return new ExceptionPrecondition(exception);
		}

		/**
		 * Convert an Error into an Exception
		 */
		static fromError(e) {
			let exception = new Exception();
			exception.message = e.message || String(e);
			exception.stack = e.stack;
			if ("topics" in e) {
				exception.topics = e.topics;
			}
			if ("nestedErrorList" in e) {
				exception.nestedErrorList = e.nestedErrorList;
			}
			return exception;
		}

		/**
		 * \brief Assert that expression evaluates to true.
		 *
		 * \param expression The expression to evaluate.
		 * \param str (optional) The message to display if the assertion fails.
		 * \param ...args (optional) Arguments to add to the message.
		 */
		static assert(expression, str = "", ...args) {
			if (!expression) {
				let value = new ExceptionCombine("Assertion failed");
				value.add(str, ...args);
				throw new Exception(value);
			}
		}

		/**
		 * \brief Assert that a precondition is satisfied.
		 *
		 * \param expression The expression to evaluate.
		 * \param str (optional) The message to display if the assertion fails.
		 * \param ...args (optional) Arguments to add to the message.
		 */
		static assertPrecondition(expression, str = "", ...args) {
			if (!expression) {
				let value = new ExceptionCombine("Precondition failed");
				value.add(str, ...args);
				throw this.makePreconditionException(value);
			}
		}

		/**
		 * Assert that the result passed into argument has a value.
		 *
		 * \param expression The result to evaluate.
		 */
		static assertResult(result) {
			if (result.hasError()) {
				let value = new ExceptionCombine("Assertion failed");
				value.add(result.error());
				throw new Exception(value);
			}
		}

		/**
		 * Assert that the result passed into argument has a value.
		 *
		 * \param expression The result to evaluate.
		 */
		static assertPreconditionResult(result) {
			if (result.hasError()) {
				let value = new ExceptionCombine("Precondition failed");
				value.add(result.error());
				throw this.makePreconditionException(value);
			}
		}

		/**
		 * \brief Assert that 2 values are equal.
		 * This is not a strict assert.
		 *
		 * \param value1 The first value.
		 * \param value2 The second value.
		 * \param str (optional) The message to display if the assertion fails.
		 * \param ...args (optional) Arguments to add to the message.
		 */
		static assertEqual(value1, value2, str = "", ...args) {
			const assertEqualInternal = (value1, value2, combine) => {
				if (typeof value1 === "object" && value1 !== null && typeof value2 === "object" && value2 !== null) {
					if (value1 instanceof Array && value2 instanceof Array) {
						Exception.assert(value1.length === value2.length, combine);
						value1.forEach((subValue1, index) => {
							assertEqualInternal(subValue1, value2[index], combine);
						});
					} else {
						assertEqualInternal(Object.keys(value1), Object.keys(value2), combine);
						Object.keys(value1).forEach((key) => {
							assertEqualInternal(value1[key], value2[key], combine);
						});
					}
				} else {
					Exception.assert(value1 == value2, combine);
				}
			};

			let combine = new ExceptionCombine("Values are not equal, value1={:j}, value2={:j}", value1, value2);
			combine.add(str, ...args);
			assertEqualInternal(value1, value2, combine);
		}

		/**
		 * Ensures that a specific block of code throws an exception
		 */
		static async assertThrows(block, str = "", ...args) {
			let hasThrown = false;

			try {
				await block();
			} catch (e) {
				hasThrown = true;
			} finally {
				let combine = new ExceptionCombine("Code block did not throw");
				combine.add(str, ...args);
				Exception.assert(hasThrown, combine);
			}
		}

		/**
		 * Error reached
		 */
		static error(str = "", ...args) {
			let combine = new ExceptionCombine("Error");
			combine.add(str, ...args);
			throw new Exception(combine);
		}

		/**
		 * Error reached
		 */
		static errorPrecondition(str = "", ...args) {
			let combine = new ExceptionCombine("Error");
			combine.add(str, ...args);
			throw this.makePreconditionException(combine);
		}

		/**
		 * Flag a line of code unreachable
		 */
		static unreachable(str = "", ...args) {
			let combine = new ExceptionCombine("Code unreachable");
			combine.add(str, ...args);
			throw new Exception(combine);
		}

		/**
		 * \brief Print a formatted exception message
		 */
		static print(...args) {
			Log.custom(
				{
					level: "error",
					topics: topics,
				},
				...args,
			);
		}

		/**
		 * \brief Print a formatted exception message
		 */
		print(...args) {
			if (args.length) {
				Exception.print(...args);
			}
			// We need a custom print to not print twice the topics.
			// Topics are printed because 'this' is an exception as well.
			Log.custom({ level: "error" }, this);
		}

		/**
		 * \brief Print the current exception object
		 */
		toString() {
			let message =
				"[" +
				this.topics.join("::") +
				"] " +
				this.name +
				(this.message ? " with message: " + String(this.message) : "") +
				// Remove the first line of the stack to avoid polluting the output
				(this.stack ? "; Callstack:\n" + String(this.stack).split("\n").slice(1).join("\n") : "");
			message += this.nestedErrorList.map((e) => "\nFrom: " + String(e));
			return message;
		}
	};
};

const E = ExceptionFactory("exception");

// Register uncaught exception handler
uncaughtExceptionHandler(E);

export default ExceptionFactory;
