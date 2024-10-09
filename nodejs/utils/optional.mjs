import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("optional");

// Unique value when no value is needed.
const emptyValue_ = Symbol("optional-empty-value");

export default class Optional {
	/// A default constructor will create an empty optional.
	constructor(value = emptyValue_) {
		this.value_ = value;
	}

	hasValue() {
		return this.value_ !== emptyValue_;
	}

	isEmpty() {
		return this.value_ === emptyValue_;
	}

	value() {
		Exception.assert(this.hasValue(), "Cannot access the value of an empty optional.");
		return this.value_;
	}

	toString() {
		if (this.hasValue()) {
			return "<Optional value=" + String(this.value_) + ">";
		}
		return "<Optional>";
	}
}
