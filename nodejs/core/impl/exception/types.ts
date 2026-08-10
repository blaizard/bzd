/// Interface of the exception class passed to the handler.
export interface ExceptionClass {
    fromError(error: unknown, message?: string | null, ...args: unknown[]): ExceptionInstance;
}

interface ExceptionInstance {
    print(...args: unknown[]): void;
}
