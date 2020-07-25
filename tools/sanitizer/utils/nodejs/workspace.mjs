import Filter from "./filter.mjs";
import FileSystem from "bzd/core/filesystem.mjs";
import ExceptionFactory from "bzd/core/exception.mjs";
import Path from "path";

const Exception = ExceptionFactory("workspace");

export default class Workspace {
	constructor(path, options) {
		this.options = Object.assign(
			{
				include: [],
				exclude: [],
			},
			options
		);
		this.include = new Filter(this.options.include);
		this.exclude = new Filter(this.options.exclude);
		this.path = path;
	}

	static async _findWorkspace(workspace) {
		let path = workspace;
		while (path) {
			if (await FileSystem.exists(Path.join(path, "WORKSPACE"))) {
				return path;
			}
			const parentPath = Path.dirname(path);
			Exception.assert(path != parentPath, "Could not find any workspace directory associated with {}", workspace);
			path = parentPath;
		}
	}

	async *data(relative = false) {
		const workspace = await Workspace._findWorkspace(this.path);
		for await (const path of FileSystem.walk(this.path)) {
			const relativePath = Path.relative(workspace, path);
			if (!this.exclude.match(relativePath)) {
				if (this.include.match(relativePath)) {
					yield relative ? relativePath : path;
				}
			}
		}
	}
}
