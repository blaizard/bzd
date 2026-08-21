import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Format from "#bzd/nodejs/core/format.js";

const Exception = ExceptionFactory("result");

// Unique value when no value is needed.
const emptyValue_ = Symbol("result-empty-value");

export default class Result<T = boolean> {
    private value_: T | boolean | typeof emptyValue_;
    private error_: unknown;

    /// A default constructor will create a result holding a value.
    constructor(value: T | boolean = true) {
        this.value_ = value;
        this.error_ = emptyValue_;
    }

    static makeError(error: unknown): Result<never> {
        const result = new Result<never>(emptyValue_ as never);
        result.error_ = error;
        return result;
    }

    static makeErrorString(str: string = "", ...args: unknown[]): Result<never> {
        const result = new Result<never>(emptyValue_ as never);
        result.error_ = Format(str, ...args);
        return result;
    }

    hasValue(): boolean {
        return this.value_ !== emptyValue_;
    }

    hasError(): boolean {
        return this.error_ !== emptyValue_;
    }

    value(): T {
        Exception.assert(
            this.hasValue(),
            "Cannot access the value of a result containing an error: {}",
            String(this.error_),
        );
        return this.value_ as T;
    }

    error(): unknown {
        Exception.assert(
            this.hasError(),
            "Cannot access the error of a result containing a value: {}",
            String(this.value_),
        );
        return this.error_;
    }

    toString(): string {
        if (this.hasValue()) {
            return "<Result value=" + String(this.value_) + ">";
        }
        return "<Result error=" + String(this.error_) + ">";
    }
}