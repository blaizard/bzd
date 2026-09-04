import Format from "#bzd/nodejs/core//format.js";
import LogFactory from "#bzd/nodejs/core//log.js";
import uncaughtExceptionHandler from "#bzd/nodejs/core/impl/exception/backend.js";

const Log = LogFactory("exception");

/// Fragment of an exception, holding a message and an optional stack.
class _ExceptionFragment {
    message: string;
    stack: string | undefined;

    constructor(message: string, stack: string | undefined) {
        this.message = message;
        this.stack = stack;
    }
}

/// Interface of an exception instance.
interface ExceptionInstance {
    e: _ExceptionFragment[];
    topics: string[];

    clone(): ExceptionInstance;
    combine(...exceptions: ExceptionInstance[]): void;
    print(...args: unknown[]): void;
    readonly message: string;
    toString(): string;
}

/// Interface of a result object.
interface ResultInterface {
    hasValue(): boolean;
    hasError(): boolean;
    value(): unknown;
    error(): unknown;
}

export class ExceptionPrecondition extends Error {
    exception: ExceptionInstance;

    constructor(exception: ExceptionInstance) {
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

export const ExceptionFactory = (...topics: string[]) => {
    return class Exception extends Error implements ExceptionInstance {
        // All the errors this exception carries. The first in the list is the earliest.
        e: _ExceptionFragment[] = [];
        // Topics associated with this object.
        topics: string[] = topics;

        constructor(messageOrException: unknown, ...args: unknown[]) {
            // Capture the call stack.
            super();

            if (messageOrException instanceof Exception) {
                this.e.unshift(...messageOrException.e);
            } else if (messageOrException instanceof Error) {
                this.e.push(new _ExceptionFragment(messageOrException.message, messageOrException.stack));
            } else if (typeof messageOrException === "string") {
                this.e.push(new _ExceptionFragment(Format(messageOrException, ...args), this.stack));
            } else {
                this.e.push(
                    new _ExceptionFragment(String(messageOrException) + args.map(String).join("; "), this.stack)
                );
            }
        }

        /// Clone the current exception instance.
        clone(): ExceptionInstance {
            let exception = new Exception("");
            exception.e = [...this.e];
            exception.topics = topics;
            return exception;
        }

        /// Combine multiple exceptions to this one.
        combine(...exceptions: ExceptionInstance[]): void {
            for (const exception of exceptions) {
                this.e.push(...exception.e);
            }
        }

        static makePreconditionException(messageOrException: unknown, ...args: unknown[]): ExceptionPrecondition {
            const exception = new Exception(messageOrException, ...args);
            return new ExceptionPrecondition(exception);
        }

        /// Convert an Error into an Exception
        static fromError(e: unknown, message: string | null = null, ...args: unknown[]): ExceptionInstance {
            const base = new Exception(e);
            if (message) {
                const update = new Exception(message, ...args);
                base.combine(update);
            }
            return base;
        }

        /// \brief Assert that expression evaluates to true.
        ///
        /// \param expression The expression to evaluate.
        /// \param message (optional) The message to display if the assertion fails.
        /// \param ...args (optional) Arguments to add to the message.
        static assert(expression: unknown, message: string = "", ...args: unknown[]): void {
            if (!expression) {
                throw new Exception("Assertion failed" + (message ? "; " + message : ""), ...args);
            }
        }

        /// \brief Assert that a precondition is satisfied.
        ///
        /// \param expression The expression to evaluate.
        /// \param message (optional) The message to display if the assertion fails.
        /// \param ...args (optional) Arguments to add to the message.
        static assertPrecondition(expression: unknown, message: string = "", ...args: unknown[]): void {
            if (!expression) {
                throw Exception.makePreconditionException(
                    "Precondition failed" + (message ? "; " + message : ""),
                    ...args
                );
            }
        }

        /// Assert that the result passed as an argument has a value.
        ///
        /// \param result The result to evaluate.
        static assertResult(result: ResultInterface): void {
            if (result.hasError()) {
                throw new Exception("Assertion failed; {}", result.error());
            }
        }

        /// Assert that the result passed as an argument has an error.
        ///
        /// \param result The result to evaluate.
        static assertResultError(result: ResultInterface): void {
            if (result.hasValue()) {
                throw new Exception("Assertion failed; result contains the value: {:?}", result.value());
            }
        }

        /// Assert that the result passed as an argument has a value.
        ///
        /// \param result The result to evaluate.
        static assertPreconditionResult(result: ResultInterface): void {
            if (result.hasError()) {
                throw Exception.makePreconditionException("Precondition failed; {}", result.error());
            }
        }

        /// \brief Assert that two values are equal.
        /// This is not a strict equality check.
        ///
        /// \param value1 The first value.
        /// \param value2 The second value.
        /// \param message (optional) The message to display if the assertion fails.
        /// \param ...args (optional) Arguments to add to the message.
        static assertEqual(value1: unknown, value2: unknown, message: string = "", ...args: unknown[]): void {
            const assertEqualInternal = (value1: unknown, value2: unknown, ...exceptionArgs: any[]): void => {
                if (typeof value1 === "object" && value1 !== null && typeof value2 === "object" && value2 !== null) {
                    if (value1 instanceof Array && value2 instanceof Array) {
                        Exception.assert(value1.length === value2.length, ...exceptionArgs);
                        value1.forEach((subValue1, index) => {
                            assertEqualInternal(subValue1, value2[index], ...exceptionArgs);
                        });
                    } else if (value1 instanceof Set && value2 instanceof Set) {
                        Exception.assert(value1.size === value2.size, ...exceptionArgs);
                        for (const element of value1) {
                            Exception.assert(value2.has(element), ...exceptionArgs);
                        }
                    } else {
                        const dictionary1 = value1 as Record<string, unknown>;
                        const dictionary2 = value2 as Record<string, unknown>;
                        assertEqualInternal(Object.keys(dictionary1), Object.keys(dictionary2), ...exceptionArgs);
                        Object.keys(dictionary1).forEach((key) => {
                            assertEqualInternal(dictionary1[key], dictionary2[key], ...exceptionArgs);
                        });
                    }
                } else {
                    Exception.assert(value1 == value2, ...exceptionArgs);
                }
            };

            assertEqualInternal(
                value1,
                value2,
                "Values are not equal, value1={:?}, value2={:?}" + (message ? "; " + message : ""),
                value1,
                value2,
                ...args
            );
        }

        /// Ensures that a specific block of code throws an exception matching a specific message.
        static async assertThrowsWithMatch(
            block: () => unknown,
            match: string,
            message: string = "",
            ...args: unknown[]
        ): Promise<void> {
            let hasThrown = false;

            try {
                await block();
            } catch (e) {
                hasThrown = true;
                Exception.assert(
                    String(e).includes(match),
                    "Code block threw but the exception message did not match the expectation: '" +
                        match +
                        "' vs '" +
                        String(e) +
                        "'."
                );
            } finally {
                Exception.assert(hasThrown, "Code block did not throw" + (message ? "; " + message : ""), ...args);
            }
        }

        /// Ensures that a specific block of code throws an exception.
        static async assertThrows(block: () => unknown, message: string = "", ...args: unknown[]): Promise<void> {
            return Exception.assertThrowsWithMatch(block, "", message, ...args);
        }

        /// Throw an error with the given message.
        static error(message: string, ...args: unknown[]): never {
            throw new Exception("Error; " + message, ...args);
        }

        /// Throw a precondition error with the given message.
        static errorPrecondition(message: string, ...args: unknown[]): never {
            throw Exception.makePreconditionException("Error; " + message, ...args);
        }

        /// Flag a line of code as unreachable.
        static unreachable(message: string, ...args: unknown[]): never {
            throw new Exception("Code unreachable; " + message, ...args);
        }

        /// Print a formatted exception message.
        static print(...args: unknown[]): void {
            Log.custom(
                {
                    level: "error",
                    topics: topics,
                },
                ...args
            );
        }

        /// Print a formatted exception message.
        print(...args: unknown[]): void {
            let exception: ExceptionInstance = this;
            if (args.length) {
                exception = this.clone();
                exception.combine(new Exception(args[0], ...args.slice(1)));
            }
            Log.custom({ level: "error", topics: exception.topics }, "{}", String(exception));
        }

        get message() {
            return this.e.map((e) => e.message).join("\n");
        }

        /// Print the current exception object.
        toString(): string {
            let content = [this.message];
            if (this.e.length > 0) {
                content.push(String(this.e[0].stack));
            }
            return content.join("\n");
        }
    };
};

const E = ExceptionFactory("exception");

// Register uncaught exception handler.
uncaughtExceptionHandler(E);

export default ExceptionFactory;
