import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("pathlib");

function _pathToArray(path) {
	return path.split("/").filter(Boolean);
}

class Path {
	constructor(maybeRoot, path) {
		this.maybeRoot = maybeRoot;
		this.path = path;
	}

	/// A string representing the final path component, excluding the drive and root, if any.
	get name() {
		return this.path.at(-1);
	}

	/// The file extension of the final component, if any.
	get suffix() {
		return this.name.split(".").at(-1);
	}

	/// The logical parent of the path.
	get parent() {
		return new Path(this.maybeRoot, this.path.slice(0, -1));
	}

	/// A list giving access to the path’s various components.
	get parts() {
		if (this.isAbsolute()) {
			return [this.maybeRoot, ...this.path];
		}
		return [...this.path];
	}

	/// Normalize the specified path. All redundant separator and up-level references are collapsed in the process of path normalization.
	get normalize() {
		let normalizedPath = [];
		for (const segment of this.path) {
			if (segment == "..") {
				const result = normalizedPath.pop();
				Exception.assert(
					result !== undefined,
					"Path cannot be normalized '{}', the '..' expands beyond the root.",
					this.path.asPosix(),
				);
			} else if (segment != ".") {
				normalizedPath.push(segment);
			}
		}
		return new Path(this.maybeRoot, normalizedPath);
	}

	/// Clone the pathlib instance.
	clone() {
		return new Path(this.maybeRoot, [...this.path]);
	}

	/// Return whether the path is absolute or not. A path is considered absolute if it has both a root and (if the flavour allows) a drive.
	isAbsolute() {
		return this.maybeRoot !== null;
	}

	/// Return a string representation of the path with forward slashes (/).
	asPosix() {
		const prefix = this.isAbsolute() ? this.maybeRoot : "";
		return prefix + this.path.join("/");
	}

	/// Represent the path as a file URI. Exception is raised if the path isn’t absolute.
	asUri() {
		Exception.assert(this.isAbsolute(), "A path used with asUri() must be absolute: {}", this.asPosix());
		return "file://" + this.maybeRoot + this.path.join("/");
	}

	/// Calling this method is equivalent to combining the path with each of the given path segments in turn.
	joinPath(...segments) {
		const clone = this.clone();
		for (const segment of segments) {
			clone.path = clone.path.concat(_pathToArray(segment));
		}
		return clone;
	}
}

const accessors = {
	path: (path) => {
		if (path instanceof Path) {
			return path;
		}
		const maybeRoot = path[0] == "/" ? "/" : null;
		return new Path(maybeRoot, _pathToArray(path));
	},
};

export default accessors;
