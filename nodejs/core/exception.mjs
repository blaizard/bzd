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

/// Private class that contains the exception information.
class _ExceptionFragment {
	constructor(message, stack) {
		this.message = message;
		this.stack = stack;
	}
}

export const ExceptionFactory = (...topics) => {
	return class Exception extends Error {
		constructor(messageOrException, ...args) {
			// This should capture a callstack
			super();

			// All the errors this exception carries. The first in the list being the earliest.
			this.e = [];
			// Topics associated with this object
			this.topics = topics;

			if (messageOrException instanceof Exception) {
				this.e.unshift(...messageOrException.e);
			} else if (messageOrException instanceof Error) {
				this.e.push(new _ExceptionFragment(messageOrException.message, messageOrException.stack));
			} else if (typeof messageOrException == "string") {
				this.e.push(new _ExceptionFragment(Format(messageOrException, ...args), this.stack));
			} else {
				this.e.push(new _ExceptionFragment(String(messageOrException) + args.map(String).join("; "), this.stack));
			}
		}

		/// Clone the current exception instance.
		clone() {
			let exception = new Exception();
			exception.e = [...this.e];
			exception.topics = topics;
			return exception;
		}

		/// Combine multiple exceptions to this one.
		combine(...exceptions) {
			for (const exception of exceptions) {
				this.e.push(...exception.e);
			}
		}

		static makePreconditionException(messageOrException, ...args) {
			const exception = new Exception(messageOrException, ...args);
			return new ExceptionPrecondition(exception);
		}

		/// Convert an Error into an Exception
		static fromError(e) {
			return new Exception(e);
		}

		/// \brief Assert that expression evaluates to true.
		///
		/// \param expression The expression to evaluate.
		/// \param str (optional) The message to display if the assertion fails.
		/// \param ...args (optional) Arguments to add to the message.
		static assert(expression, message = "", ...args) {
			if (!expression) {
				throw new Exception("Assertion failed" + (message ? "; " + message : ""), ...args);
			}
		}

		/// \brief Assert that a precondition is satisfied.
		///
		/// \param expression The expression to evaluate.
		/// \param str (optional) The message to display if the assertion fails.
		/// \param ...args (optional) Arguments to add to the message.
		static assertPrecondition(expression, message = "", ...args) {
			if (!expression) {
				throw this.makePreconditionException("Precondition failed" + (message ? "; " + message : ""), ...args);
			}
		}

		/// Assert that the result passed into argument has a value.
		///
		// \param expression The result to evaluate.
		static assertResult(result) {
			if (result.hasError()) {
				throw new Exception("Assertion failed; " + result.error(), ...args);
			}
		}

		/// Assert that the result passed into argument has a value.
		///
		/// \param expression The result to evaluate.
		static assertPreconditionResult(result) {
			if (result.hasError()) {
				throw this.makePreconditionException("Precondition failed; " + result.error());
			}
		}

		/// \brief Assert that 2 values are equal.
		/// This is not a strict assert.
		///
		/// \param value1 The first value.
		/// \param value2 The second value.
		/// \param message (optional) The message to display if the assertion fails.
		/// \param ...args (optional) Arguments to add to the message.
		static assertEqual(value1, value2, message = "", ...args) {
			const assertEqualInternal = (value1, value2, exception) => {
				if (typeof value1 === "object" && value1 !== null && typeof value2 === "object" && value2 !== null) {
					if (value1 instanceof Array && value2 instanceof Array) {
						Exception.assert(value1.length === value2.length, exception);
						value1.forEach((subValue1, index) => {
							assertEqualInternal(subValue1, value2[index], exception);
						});
					} else {
						assertEqualInternal(Object.keys(value1), Object.keys(value2), exception);
						Object.keys(value1).forEach((key) => {
							assertEqualInternal(value1[key], value2[key], exception);
						});
					}
				} else {
					Exception.assert(value1 == value2, exception);
				}
			};

			const exception = new Exception(
				"Values are not equal, value1={:j}, value2={:j}" + (message ? "; " + message : ""),
				value1,
				value2,
				...args,
			);
			assertEqualInternal(value1, value2, exception);
		}

		/// Ensures that a specific block of code throws an exception
		static async assertThrows(block, message = "", ...args) {
			let hasThrown = false;

			try {
				await block();
			} catch (e) {
				hasThrown = true;
			} finally {
				Exception.assert(hasThrown, "Code block did not throw" + (message ? "; " + message : ""), ...args);
			}
		}

		/// Error reached
		static error(message, ...args) {
			throw new Exception("Error; " + message, ...args);
		}

		/// Error reached
		static errorPrecondition(message, ...args) {
			throw this.makePreconditionException("Error; " + message, ...args);
		}

		/// Flag a line of code unreachable
		static unreachable(message, ...args) {
			throw new Exception("Code unreachable; " + message, ...args);
		}

		/// Print a formatted exception message
		static print(...args) {
			Log.custom(
				{
					level: "error",
					topics: topics,
				},
				...args,
			);
		}

		//// Print a formatted exception message
		print(...args) {
			let exception = this;
			if (args.length) {
				exception = this.clone();
				exception.combine(new Exception(...args));
			}
			Log.custom({ level: "error", topics: exception.topics }, String(exception));
		}

		get message() {
			return this.e.map((e) => e.message).join("\n");
		}

		/// Print the current exception object
		toString() {
			let content = [this.message];
			if (this.e.length > 0) {
				content.push(String(this.e[0].stack));
			}
			return content.join("\n");
		}
	};
};

const E = ExceptionFactory("exception");

// Register uncaught exception handler
uncaughtExceptionHandler(E);

export default ExceptionFactory;
