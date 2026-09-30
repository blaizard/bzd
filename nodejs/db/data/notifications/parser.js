import ExceptionFactory from "#bzd/nodejs/core/exception.js";

const Exception = ExceptionFactory("db", "data", "notifications", "parser");

/// A variable segment is the entire segment enclosed in braces, e.g. "{name}".
const VARIABLE_REGEXPR = /^\{([a-zA-Z0-9_]+)\}$/;

const isDigit_ = (c) => c >= "0" && c <= "9";
const isLetter_ = (c) => (c >= "a" && c <= "z") || (c >= "A" && c <= "Z");
const isPathCharacter_ = (c) =>
	isLetter_(c) || isDigit_(c) || c === "_" || c === "-" || c === "." || c === "{" || c === "}";

/// The comparison operators, mapping a symbol to its implementation.
const COMPARISONS = {
	">": (l, r) => l > r,
	"<": (l, r) => l < r,
	">=": (l, r) => l >= r,
	"<=": (l, r) => l <= r,
	"==": (l, r) => l === r,
	"!=": (l, r) => l !== r,
};

/// The argument types a function can take.
const ArgumentType = {
	/// A comparison, e.g. "max(/data/cpu/{name}) > 0.95".
	comparison: (parser) => parser.parseComparisonRequired_(),
	/// A literal number, e.g. "3600".
	number: (parser) => parser.parseNumberLiteral_(),
	/// A path, e.g. "/data/cpu/{name}".
	path: (parser) => parser.parsePath_(),
};

/// The supported functions, mapping a name to its argument list and compile function.
const FUNCTIONS = {
	avg: {
		args: [ArgumentType.path],
		compile: (evaluate) => (ctx) => {
			const values = evaluate(ctx);
			if (values === undefined) return undefined;
			let sum = 0;
			for (const value of values) sum += value;
			return sum / values.length;
		},
	},
	duration: {
		args: [ArgumentType.comparison, ArgumentType.number],
		compile: (condition, seconds) => {
			const secondsUs = seconds * 1000000;
			return (ctx) => {
				const result = condition(ctx);
				if (result) {
					if (ctx.memory.durationSince === null) {
						ctx.memory.durationSince = ctx.timestamp;
						return false;
					}
					return ctx.timestamp - ctx.memory.durationSince >= secondsUs;
				}
				ctx.memory.durationSince = null;
				return false;
			};
		},
	},
	latest: {
		args: [ArgumentType.path],
		compile: (evaluate) => evaluate,
	},
	max: {
		args: [ArgumentType.path],
		compile: (evaluate) => (ctx) => {
			const values = evaluate(ctx);
			if (values === undefined) return undefined;
			let result = -Infinity;
			for (const value of values) if (value > result) result = value;
			return result;
		},
	},
	min: {
		args: [ArgumentType.path],
		compile: (evaluate) => (ctx) => {
			const values = evaluate(ctx);
			if (values === undefined) return undefined;
			let result = Infinity;
			for (const value of values) if (value < result) result = value;
			return result;
		},
	},
	sum: {
		args: [ArgumentType.path],
		compile: (evaluate) => (ctx) => {
			const values = evaluate(ctx);
			if (values === undefined) return undefined;
			let result = 0;
			for (const value of values) result += value;
			return result;
		},
	},
};

export default class SignalParser {
	/// Parse a signal into a compiled evaluator.
	///
	/// The parsing is done once at the application initialization so that any
	/// syntax or schema issue is detected as soon as possible, and the resulting
	/// evaluator can then be used directly at runtime without any further parsing.
	///
	/// \param signal The signal to parse, e.g. "max(/data/cpu/{name}) > 0.95".
	///
	/// \return An object containing:
	///         - dependencies: the list of unique paths (as key arrays) referenced by the signal.
	///         - variable: the single variable name if the signal has one, null otherwise.
	///         - evaluate: the evaluator function, to be called with a context.
	static parse(signal) {
		return new SignalParser(signal).parse_();
	}

	constructor(signal) {
		Exception.assertPrecondition(typeof signal === "string", "The signal must be a string, not: {:?}", signal);
		this.signal_ = signal;
		this.index_ = 0;
		this.dependencies_ = [];
		this.dependenciesSeen_ = new Set();
		this.variables_ = new Set();
	}

	/// Throw a parse error at the current position.
	error_(message) {
		Exception.errorPrecondition("Invalid signal '{}': {} at position {}.", this.signal_, message, this.index_);
	}

	/// Get the character at the current position, without advancing.
	peek_(offset = 0) {
		return this.signal_[this.index_ + offset];
	}

	/// Skip whitespaces.
	skipWhitespace_() {
		while (this.index_ < this.signal_.length && /\s/.test(this.signal_[this.index_])) {
			++this.index_;
		}
	}

	/// Parse the whole signal.
	parse_() {
		this.skipWhitespace_();
		if (this.index_ >= this.signal_.length) {
			this.error_("the signal is empty");
		}
		const evaluate = this.parseComparison_();
		this.skipWhitespace_();
		if (this.index_ < this.signal_.length) {
			this.error_("unexpected trailing content '" + this.signal_.slice(this.index_) + "'");
		}
		if (this.variables_.size > 1) {
			this.error_("a signal can only have one variable, got: " + Array.from(this.variables_).join(", "));
		}
		return {
			dependencies: this.dependencies_,
			variable: this.variables_.size === 0 ? null : Array.from(this.variables_)[0],
			evaluate: evaluate,
		};
	}

	/// Parse a comparison, which is a value optionally followed by a comparison operator and a value.
	parseComparison_() {
		const left = this.parseValue_();
		this.skipWhitespace_();
		const operator = this.tryParseComparisonOperator_();
		if (operator === null) {
			return left;
		}
		return this.makeCompare_(operator, left, this.parseValue_());
	}

	/// Same as parseComparison_ but the comparison operator is mandatory.
	parseComparisonRequired_() {
		const left = this.parseValue_();
		this.skipWhitespace_();
		const operator = this.tryParseComparisonOperator_();
		if (operator === null) {
			this.error_("expected a comparison operator");
		}
		return this.makeCompare_(operator, left, this.parseValue_());
	}

	/// Compile a comparison between two values.
	makeCompare_(operator, left, right) {
		const compare = COMPARISONS[operator];
		return (ctx) => {
			const l = left(ctx);
			if (l === undefined) return false;
			const r = right(ctx);
			if (r === undefined) return false;
			return compare(l, r);
		};
	}

	/// Parse a value, which is a number, a path or a function call.
	parseValue_() {
		this.skipWhitespace_();
		const c = this.peek_();
		if (c === undefined) {
			this.error_("unexpected end of the signal, expected a value");
		}
		if (c === "/") {
			return this.parsePath_();
		}
		if (isDigit_(c)) {
			return this.parseNumber_();
		}
		if (isLetter_(c) || c === "_") {
			return this.parseFunction_();
		}
		this.error_("unexpected character '" + c + "'");
	}

	/// Parse a number.
	parseNumber_() {
		const value = this.scanNumber_();
		return () => value;
	}

	/// Consume a number and return its numeric value.
	scanNumber_() {
		let value = "";
		while (isDigit_(this.peek_())) {
			value += this.peek_();
			++this.index_;
		}
		if (this.peek_() === ".") {
			value += this.peek_();
			++this.index_;
			while (isDigit_(this.peek_())) {
				value += this.peek_();
				++this.index_;
			}
		}
		return Number(value);
	}

	/// Parse a literal number.
	parseNumberLiteral_() {
		this.skipWhitespace_();
		if (!isDigit_(this.peek_())) {
			this.error_("expected a literal number");
		}
		return this.scanNumber_();
	}

	/// Parse a path, which starts with '/'.
	parsePath_() {
		if (this.peek_() !== "/") {
			this.error_("expected a path");
		}
		++this.index_;
		const segments = [];
		let segment = "";
		while (this.index_ < this.signal_.length) {
			const c = this.signal_[this.index_];
			if (c === "/") {
				segments.push(segment);
				segment = "";
				++this.index_;
			} else if (isPathCharacter_(c)) {
				segment += c;
				++this.index_;
			} else {
				break;
			}
		}
		segments.push(segment);
		if (segments.length === 1 && segments[0] === "") {
			this.error_("the path is empty");
		}

		// Convert the segments into a key, extracting the variable placeholders.
		let variable = null;
		const key = [];
		for (const segment of segments) {
			const match = segment.match(VARIABLE_REGEXPR);
			if (match) {
				variable = match[1];
				key.push("{" + match[1] + "}");
				this.variables_.add(match[1]);
			} else {
				if (segment.includes("{") || segment.includes("}")) {
					this.error_("the path segment '" + segment + "' is not a valid variable");
				}
				if (segment === "." || segment === "..") {
					this.error_("the path segment '" + segment + "' is not valid");
				}
				if (segment === "") {
					this.error_("the path contains an empty segment");
				}
				key.push(segment);
			}
		}

		// Record the dependency, ignoring duplicates.
		const keyString = key.join("\x01");
		if (!this.dependenciesSeen_.has(keyString)) {
			this.dependenciesSeen_.add(keyString);
			this.dependencies_.push(key);
		}

		// Compile the evaluator: the concrete internal key is built with a single
		// string concatenation using the pre-computed literal prefix and suffix.
		if (variable === null) {
			const internalKey = key.join("\x01");
			return (ctx) => ctx.getValue(internalKey);
		}
		const variableIndex = key.indexOf("{" + variable + "}");
		const internalPrefix = variableIndex === 0 ? "" : key.slice(0, variableIndex).join("\x01") + "\x01";
		const suffix = key.slice(variableIndex + 1);
		const internalSuffix = suffix.length === 0 ? "" : "\x01" + suffix.join("\x01");
		return (ctx) => ctx.getValue(internalPrefix + ctx.variant + internalSuffix);
	}

	/// Parse a function call.
	parseFunction_() {
		let name = "";
		while (isLetter_(this.peek_()) || isDigit_(this.peek_()) || this.peek_() === "_") {
			name += this.peek_();
			++this.index_;
		}
		this.skipWhitespace_();

		const spec = FUNCTIONS[name];
		if (spec === undefined) {
			this.error_("unknown function '" + name + "'");
		}
		if (this.peek_() !== "(") {
			this.error_("expected '(' after function '" + name + "'");
		}
		++this.index_;

		// Parse the arguments according to their declared types.
		const args = [];
		for (let index = 0; index < spec.args.length; ++index) {
			this.skipWhitespace_();
			if (index > 0) {
				if (this.peek_() !== ",") {
					this.error_("expected ',' in function '" + name + "'");
				}
				++this.index_;
				this.skipWhitespace_();
			}
			args.push(spec.args[index](this));
		}
		this.skipWhitespace_();
		if (this.peek_() !== ")") {
			this.error_("expected ')' to close the function '" + name + "'");
		}
		++this.index_;

		return spec.compile(...args);
	}

	/// Try to parse a comparison operator, without advancing on failure.
	tryParseComparisonOperator_() {
		const c = this.peek_();
		if (c === ">" || c === "<") {
			if (this.peek_(1) === "=") {
				this.index_ += 2;
				return c + "=";
			}
			++this.index_;
			return c;
		}
		if (c === "=" || c === "!") {
			if (this.peek_(1) === "=") {
				this.index_ += 2;
				return c + "=";
			}
			return null;
		}
		return null;
	}
}
