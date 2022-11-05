import ExceptionFactory from "bzd/core/exception.mjs";
import Vue from "vue";

const Exception = ExceptionFactory("filesystem");

class Node {
	constructor(filesystem, node, path) {
		this.filesystem = filesystem;
		this.node = node;
		this.path = path;
		this.name = this.node.name;
		this.content = "";
	}

	async getChildren() {
		return this.node.children;
	}

	async fetchContent() {
		Exception.assert(FileSystem.isFile(this.node), "Folder do not have content");
		this.content = await this.filesystem.api.request("get", "/file/content", {
			path: this.path,
		});
	}

	async updateContent(process) {
		await this.fetchContent();
		await process(this);
		await this.filesystem.api.request("post", "/file/content", { path: this.path, content: this.content });
	}

	async updateName(process) {
		await process(this);
		// Update the file name.
	}
}

export default class FileSystem {
	constructor(api) {
		this.api = api;
		// Internal structure of the file tree, it looks like this:
		// {
		//     uid: {name: "diplay name", expanded: true, children: { ... }}, <- for a folder
		//     uid: {name: "diplay name", content: ""}, <- for a file
		//     ...
		// }
		this.tree = {};
		// Selected node if any.
		this.selected = null;
	}

	/// Create a renderable tree from the data.
	makeTree() {
		const makeNode = (tree, parent = null) => {
			return Object.entries(tree).map(([, node]) => {
				let output = Object.assign({ selected: this.selected && this.selected.node === node, parent: parent }, node);
				if (FileSystem.isFolder(node)) {
					output.children = makeNode(node.children, output);
				}
				return output;
			});
		};
		return makeNode(this.tree);
	}

	/// Get the file or folder name from a path.
	static basename(path) {
		return FileSystem.split_(path).at(-1);
	}

	/// Get the directory name from a path.
	static dirname(path) {
		const split = FileSystem.split_(path);
		if (split.length <= 1) {
			return "";
		}
		return split.slice(0, -1).join("/");
	}

	/// Split a path into folders and file.
	static split_(path) {
		return path
			.trim()
			.split("/")
			.filter((name) => name.length > 0);
	}

	/// Create a new file at a given path.
	///
	/// \return The node representing the file created.
	async createFile(path, initialName = null) {
		const dirname = FileSystem.dirname(path);
		const basename = FileSystem.basename(path);
		const current = await this.createFolder(dirname);
		Exception.assert(!(basename in current), "File {} already exists!", path);
		Vue.set(current, basename, { name: initialName === null ? basename : initialName, content: "" });
		await this.api.request("post", "/file/content", { path: path });
		this.selected = this.makeNode_(current[basename], path);
	}

	/// Create a new folder at a given path.
	async createFolder(path) {
		let current = this.tree;
		for (const name of FileSystem.split_(path)) {
			if (name in current) {
				Exception.assert(FileSystem.isFolder(current[name]), "Folder path conflicts with an existing file: {}", path);
			}
			else {
				Vue.set(current, name, { name: name, expanded: true, children: {} });
			}
			this.selected = this.makeNode_(current[name], path);
			current = await this.selected.getChildren();
		}
		return current;
	}

	exists(path) {
		return FileSystem.get(path) !== null;
	}

	/// Get a node refered at a specified path.
	select(path) {
		let current = { children: this.tree };
		for (const name of FileSystem.split_(path)) {
			if (!FileSystem.isFolder(current)) {
				return null;
			}
			if (!(name in current.children)) {
				return null;
			}
			current = current.children[name];
		}
		this.selected = this.makeNode_(current, path);
		return current;
	}

	/// Checks if the node is a directory
	static isFolder(node) {
		return "children" in node;
	}

	/// Checks if the node is a file
	static isFile(node) {
		return !FileSystem.isFolder(node);
	}

	makeNode_(node, path) {
		return new Node(this, node, path);
	}
}
