import Filter from "./filter.mjs";
import FileSystem from "bzd/core/filesystem.mjs";
import ExceptionFactory from "bzd/core/exception.mjs";
import Path from "path";
import { fileURLToPath } from "url";

const __filename = fileURLToPath(import.meta.url);
const Exception = ExceptionFactory("workspace");

export default class Workspace {
	constructor(path, options) {
		this.options = Object.assign(
			{
				include: [],
				exclude: [],
			},
			options,
		);
		Exception.assert(typeof path == "string", "Path must be a string: {}", path);
		this.path = path;
		this.memoization = null;
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

	async _getData() {
		if (this.memoization === null) {
			const configPath = Path.join(Path.dirname(__filename), "..", "..", ".sanitizer.json");
			const config = JSON.parse(await FileSystem.readFile(configPath));
			this.memoization = {
				workspace: await Workspace._findWorkspace(this.path),
				include: new Filter(this.options.include),
				exclude: new Filter(this.options.exclude.concat(config.exclude || [])),
			};
		}
		return this.memoization;
	}

	async *data(relative = false) {
		const { workspace, exclude, include } = await this._getData();
		for await (const path of FileSystem.walk(this.path)) {
			const relativePath = Path.relative(workspace, path);
			if (!exclude.match(relativePath)) {
				if (include.match(relativePath)) {
					yield relative ? relativePath : path;
				}
			}
		}
	}
}
