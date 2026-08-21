import ExceptionFactory from "#bzd/nodejs/core/exception.js";

const Exception = ExceptionFactory("optional");

// Unique value when no value is needed.
const emptyValue_ = Symbol("optional-empty-value");

export default class Optional<T> {
    private value_: T | typeof emptyValue_;

    /// A default constructor will create an empty optional.
    constructor(value: T | typeof emptyValue_ = emptyValue_) {
        this.value_ = value;
    }

    hasValue(): boolean {
        return this.value_ !== emptyValue_;
    }

    isEmpty(): boolean {
        return this.value_ === emptyValue_;
    }

    value(): T {
        Exception.assert(this.hasValue(), "Cannot access the value of an empty optional.");
        return this.value_ as T;
    }

    toString(): string {
        if (this.hasValue()) {
            return "<Optional value=" + String(this.value_) + ">";
        }
        return "<Optional>";
    }
}