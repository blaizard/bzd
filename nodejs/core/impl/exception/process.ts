import type { ExceptionClass } from "#bzd/nodejs/core/impl/exception/types.js";

/// Handle uncaught exceptions and unhandled rejections for the Node.js process.
export default function uncaughtExceptionHandler(ExceptionFactory: ExceptionClass): void {
    process.on("uncaughtException", (e) => {
        ExceptionFactory.fromError(e).print("Exception: uncaughtException");
        process.exit(1);
    });
    process.on("unhandledRejection", (reason, promise) => {
        ExceptionFactory.fromError(reason).print("Exception: unhandledRejection, promise: '{}'", promise);
        process.exit(2);
    });
}
