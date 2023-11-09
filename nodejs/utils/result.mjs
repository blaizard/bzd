import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Format from "#bzd/nodejs/core/format.mjs";

const Exception = ExceptionFactory("result");

export default class Result {
	/// A default constructor will create a result holding a value.
	constructor(value = true) {
		this.value_ = value;
		this.error_ = undefined;
	}

	static makeError(error) {
		let result = new Result(undefined);
		result.error_ = error;
		return result;
	}

	static makeErrorString(str = "", ...args) {
		let result = new Result(undefined);
		result.error_ = Format(str, ...args);
		return result;
	}

	hasValue() {
		return this.value_ !== undefined;
	}

	hasError() {
		return this.error_ !== undefined;
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
