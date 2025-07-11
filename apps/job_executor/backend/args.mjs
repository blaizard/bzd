import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("backend", "args");

/// Convert an argument pattern to a list.
///
/// A pattern might looks like this:
///   [["--key", "{key}"], "{hello}"]
/// This implies that ["--key", "{key}"] is conditional and will be repeated for as many "key" as there is.
export default class Args {
	constructor(pattern, data) {
		this.pattern = pattern;
		this.data = data;
		this.keyRegex = new RegExp("{([^}:]*)(?::([^}]*))?}", "g");
	}

	/// Get the direct dependencies of the pattern.
	getDependency(pattern) {
		let dependencies = new Set();
		for (const item of pattern) {
			// Only strings are direct dependencies.
			if (typeof item === "string") {
				for (const match of item.matchAll(this.keyRegex)) {
					dependencies.add(match[1]);
				}
			}
		}
		return dependencies;
	}

	process(pattern = this.pattern) {
		Exception.assert(Array.isArray(pattern), "Pattern must be an array: {}", pattern);

		let command = [];
		const dependencies = [...this.getDependency(pattern)];
		Exception.assert(
			dependencies.length == 1,
			"There can only be a single dependency per level: {}, got: {}",
			pattern,
			dependencies,
		);
		const dependency = dependencies[0];
		let values = this.data[dependency] ?? [];
		values = Array.isArray(values) ? values : [values];
		for (const value of values) {
			const replacer = (_, key, metadata) => {
				Exception.assert(key == dependency, "Key '{}' must be a dependency: {}", key, dependencies);
				// ignore metadata for now
				return value;
			};

			for (const item of pattern) {
				if (Array.isArray(item)) {
					command.push(...this.process(item));
				} else {
					Exception.assert(typeof item === "string", "Item in pattern must be a string: {}", item);
					command.push(item.replace(this.keyRegex, replacer));
				}
			}
		}
		return command;
	}
}
