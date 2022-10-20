import ExceptionFactory from "bzd/core/exception.mjs";
import Vue from "vue";

const Exception = ExceptionFactory("filesystem");

export default class FileSystem {
	constructor() {
		// Internal structure of the file tree, it looks like this:
		// {
		//     uid: {name: "diplay name", expanded: true, children: { ... }},
		//     ...
		// }
		this.tree = {};
	}

	/// Create a renderable tree from the data.
	makeTree() {
		const makeNode = (tree) => {
			return Object.entries(tree).map(([, node]) => {
				let output = Object.assign({}, node);
				if (FileSystem.isFolder(node)) {
					output.children = makeNode(node.children);
				}
				return output;
			});
		};
		return makeNode(this.tree);
	}

	/// Get the file or folder name from a path.
	basename(path) {
		return this.split(path).at(-1);
	}

	/// Get the directory name from a path.
	dirname(path) {
		const split = this.split(path);
		if (split.length <= 1) {
			return "";
		}
		return split.slice(0, -1).join("/");
	}

	/// Split a path into folders and file.
	split(path) {
		return path
			.trim()
			.split("/")
			.filter((name) => name.length > 0);
	}

	/// Create a new file at a given path.
	///
	/// \return The node representing the file created.
	async createFile(path) {
		const dirname = this.dirname(path);
		const basename = this.basename(path);
		const current = await this.createFolder(dirname);
		Exception.assert(!(basename in current), "File {} already exists!", path);
		Vue.set(current, basename, { name: basename });
		return current[basename];
	}

	/// Create a new folder at a given path.
	async createFolder(path) {
		let current = this.tree;
		for (const name of this.split(path)) {
			if (name in current) {
				Exception.assert(FileSystem.isFolder(current[name]), "Folder path conflicts with an existing file: {}", path);
			}
			else {
				Vue.set(current, name, { name: name, expanded: true, children: {} });
			}
			current = current[name].children;
		}
		return current;
	}

	/// Checks if the node is a directory
	static isFolder(node) {
		return "children" in node;
	}

	/// Checks if the node is a file
	static isFile(node) {
		return !this.isFolder(node);
	}
}
