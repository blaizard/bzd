import ExceptionFactory from "bzd/core/exception.mjs";
import Vue from "vue";

const Exception = ExceptionFactory("filesystem");

class Node {
	constructor(filesystem, node) {
		this.filesystem = filesystem;
		this.node = node;
		this.name = this.node.name;
		this.content = "";
	}

	async getChildren() {
		return this.node.children;
	}

	async fetchContent() {
		Exception.assert(FileSystem.isFile(this.node), "Folder do not have content");
		this.content = await this.filesystem.api.request("get", "/file/content", {
			path: this.node.path,
		});
	}

	async updateContent(process) {
		await this.fetchContent();
		await process(this);
		await this.filesystem.api.request("post", "/file/content", { path: this.node.path, content: this.content });
	}

	async toggleExpand() {
		Exception.assert(FileSystem.isFolder(this.node), "Only folders can be expanded");
		this.node.expanded = !this.node.expanded;
	}
}

export default class FileSystem {
	constructor(api) {
		this.api = api;
		// Internal structure of the file tree, it looks like this:
		// {
		//     uid: {name: "diplay name", expanded: true, path: "", children: { ... }}, <- for a folder
		//     uid: {name: "diplay name", path: "", content: ""}, <- for a file
		//     ...
		// }
		this.tree = { children: {} };
		// Selected node if any.
		this.selected = null;
	}

	/// Create a renderable tree data.
	get data() {
		return this.tree.children;
	}

	/// Get the file or folder name from a path.
	static basename(path) {
		return FileSystem.toPathList(path).at(-1);
	}

	/// Get the directory name from a path.
	static dirname(path) {
		const split = FileSystem.toPathList(path);
		if (split.length <= 1) {
			return "";
		}
		return split.slice(0, -1).join("/");
	}

	/// Split a path into folders and file.
	static toPathList(path) {
		return path
			.trim()
			.split("/")
			.filter((name) => name.length > 0);
	}

	/// Update the entries for a certain path.
	async list(path) {
		// Select the node
		let node = this.select(path, /*select*/ false);
		Exception.assert(FileSystem.isFolder(node), "The file to be listed must be a folder: {}", path);

		let next = 1000;
		let list = [];

		do {
			const response = await this.api.request("get", "/file/list", {
				path: path,
				paging: next,
			});
			next = response.next;
			list = list.concat(response.data);
		} while (next);

		const previous = node.children || {};
		Vue.set(
			node,
			"children",
			list.reduce((obj, item) => {
				let data = { name: item.name, path: path + "/" + item.name };
				if (item.type == "directory") {
					data["expanded"] = (previous[item.name] || { expanded: false }).expanded;
					data["children"] = async () => {
						await this.list(data.path);
					};
				}
				Vue.set(obj, item.name, data);
				return obj;
			}, {})
		);
	}

	/// Create a new file at a given path.
	///
	/// \return The node representing the file created.
	async createFile(path, initialName = null) {
		const dirname = FileSystem.dirname(path);
		const basename = FileSystem.basename(path);
		const current = await this.createFolder(dirname);
		Exception.assert(!(basename in current), "File {} already exists!", path);
		Vue.set(current, basename, { name: initialName === null ? basename : initialName, path: path, content: "" });
		await this.api.request("post", "/file/content", { path: path });
		this.selected = this.makeNode_(current[basename]);
	}

	/// Create a new folder at a given path.
	async createFolder(path) {
		let current = this.tree.children;
		let currentPathList = [];
		for (const name of FileSystem.toPathList(path)) {
			currentPathList.push(name);
			if (name in current) {
				Exception.assert(FileSystem.isFolder(current[name]), "Folder path conflicts with an existing file: {}", path);
			}
			else {
				Vue.set(current, name, { name: name, path: currentPathList.join("/"), expanded: true, children: {} });
			}
			this.selected = this.makeNode_(current[name]);
			current = await this.selected.getChildren();
		}
		return current;
	}

	/// Get a node refered at a specified path.
	select(path, select = true) {
		let current = this.tree;
		for (const name of FileSystem.toPathList(path)) {
			if (!FileSystem.isFolder(current)) {
				return null;
			}
			if (!(name in current.children)) {
				return null;
			}
			current = current.children[name];
		}
		if (select) {
			this.selected = this.makeNode_(current);
		}
		return current;
	}

	/// Checks if the node is a directory
	static isFolder(node) {
		return node && "children" in node;
	}

	/// Checks if the node is a file
	static isFile(node) {
		return node && !("children" in node);
	}

	makeNode_(node) {
		return new Node(this, node);
	}
}
