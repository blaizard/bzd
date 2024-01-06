import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Format from "#bzd/nodejs/core/format.mjs";

const Exception = ExceptionFactory("result");

// Unique value when no value is needed.
const emptyValue_ = Symbol("result-empty-value");

export default class Result {
	/// A default constructor will create a result holding a value.
	constructor(value = true) {
		this.value_ = value;
		this.error_ = emptyValue_;
	}

	static makeError(error) {
		let result = new Result(emptyValue_);
		result.error_ = error;
		return result;
	}

	static makeErrorString(str = "", ...args) {
		let result = new Result(emptyValue_);
		result.error_ = Format(str, ...args);
		return result;
	}

	hasValue() {
		return this.value_ !== emptyValue_;
	}

	hasError() {
		return this.error_ !== emptyValue_;
	}

	value() {
		Exception.assert(
			this.hasValue(),
			"Cannot access the value of a result containing an error: {}",
			String(this.error_),
		);
		return this.value_;
	}

	error() {
		Exception.assert(
			this.hasError(),
			"Cannot access the error of a result containing a value: {}",
			String(this.value_),
		);
		return this.error_;
	}

	toString() {
		if (this.hasValue()) {
			return "<Result value=" + String(this.value_) + ">";
		}
		return "<Result error=" + String(this.error_) + ">";
	}
}
