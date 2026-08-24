import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import * as Os from "os";

const Exception = ExceptionFactory("pathlib");

class Path {
    constructor(maybeRoot: string | null, path: string | string[]) {
        this.maybeRoot = maybeRoot;
        this.path = Path._pathToArray(path);
    }

    private maybeRoot: string | null;
    private path: string[];

    private static _pathToArray(path: string | string[] | Path): string[] {
        if (path instanceof Path) {
            return path.path;
        }
        if (Array.isArray(path)) {
            for (const segment of path) {
                Exception.assertPrecondition(
                    typeof segment === "string" && !segment.includes("/"),
                    "Path segment '{}' is invalid: a segment cannot contain a path separator.",
                    segment,
                );
            }
            return path.filter(Boolean);
        }
        return path.split("/").filter(Boolean);
    }

    /// A string representing the final path component, excluding the drive and root, if any.
    get name(): string | undefined {
        return this.path.at(-1);
    }

    /// The file extension of the final component, if any.
    get suffix(): string | undefined {
        return this.name!.split(".").at(-1);
    }

    /// The logical parent of the path.
    get parent(): Path {
        return new Path(this.maybeRoot, this.path.slice(0, -1));
    }

    /// A list giving access to the path's various components.
    get parts(): string[] {
        if (this.isAbsolute()) {
            return [this.maybeRoot!, ...this.path];
        }
        return [...this.path];
    }

    /// Normalize the specified path. All redundant separator and up-level references are collapsed in the process of path normalization.
    get normalize(): Path {
        let normalizedPath: string[] = [];
        for (const segment of this.path) {
            if (segment == "..") {
                const result = normalizedPath.pop();
                Exception.assertPrecondition(
                    result !== undefined,
                    "Path cannot be normalized '{}', the '..' expands beyond the root.",
                    this.asPosix(),
                );
            } else if (segment != ".") {
                normalizedPath.push(segment);
            }
        }
        return new Path(this.maybeRoot, normalizedPath);
    }

    /// Clone the pathlib instance.
    clone(): Path {
        return new Path(this.maybeRoot, [...this.path]);
    }

    /// Make the path absolute, without normalization or resolving symlinks. Returns a new path object.
    absolute(): Path {
        if (this.isAbsolute()) {
            return this.clone();
        }
        return accessors.cwd().joinPath(...this.path);
    }

    /// Return whether the path is absolute or not. A path is considered absolute if it has both a root and (if the flavour allows) a drive.
    isAbsolute(): boolean {
        return this.maybeRoot !== null;
    }

    /// Return a string representation of the path with forward slashes (/).
    asPosix(): string {
        const prefix = this.isAbsolute() ? this.maybeRoot : "";
        return prefix + this.path.join("/");
    }

    /// Represent the path as a file URI. Exception is raised if the path isn't absolute.
    asUri(): string {
        Exception.assert(this.isAbsolute(), "A path used with asUri() must be absolute: {}", this.asPosix());
        return "file://" + this.maybeRoot + this.path.join("/");
    }

    /// Calling this method is equivalent to combining the path with each of the given path segments in turn.
    joinPath(...segments: (string | string[] | Path)[]): Path {
        const clone = this.clone();
        for (const segment of segments) {
            clone.path = clone.path.concat(Path._pathToArray(segment));
        }
        return clone;
    }
}

const accessors = {
    PathType: Path,
    path: (path: string | string[] | Path): Path => {
        if (path instanceof Path) {
            return path;
        }
        const maybeRoot = path[0] == "/" ? "/" : null;
        return new Path(maybeRoot, path);
    },
    /// Return the current working directory as a Path object.
    cwd(): Path {
        return accessors.path(process.cwd());
    },
    /// Return the temporary directory path.
    tmp(): Path {
        return accessors.path(Os.tmpdir());
    },
};

export default accessors;
