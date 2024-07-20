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
		for (const segment of segments) {
			this.path = this.path.concat(_pathToArray(segment));
		}
		return this;
	}
}

const accessors = {
	path: (pathStr) => {
		const maybeRoot = pathStr[0] == "/" ? "/" : null;
		return new Path(maybeRoot, _pathToArray(pathStr));
	},
};

export default accessors;
