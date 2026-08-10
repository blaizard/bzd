import type { ExceptionClass } from "#bzd/nodejs/core/impl/exception/types.js";

/// Handle uncaught exceptions for the browser window.
export default function uncaughtExceptionHandler(ExceptionFactory: ExceptionClass): void {
    window.addEventListener("error", (e) => {
        ExceptionFactory.fromError(e).print();
        return false;
    });
}
