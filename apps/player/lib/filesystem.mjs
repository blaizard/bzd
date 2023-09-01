import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Vue from "vue";

const Exception = ExceptionFactory("filesystem");

class Node {
	constructor(filesystem, node) {
		this.filesystem = filesystem;
		this.node = node;
		this.content = null;
	}

	get name() {
		return this.node.name;
	}

	get expanded() {
		return this.node.expanded || false;
	}

	get path() {
		return this.node.path;
	}

	get cursor() {
		if (typeof this.node.cursor == "number") {
			return this.node.cursor;
		}
		return this.content === null ? 0 : this.content.length;
	}

	/// Refresh the children.
	async refresh() {
		await this.filesystem.list(this.node.path);
	}

	async *get() {
		if (this.node.children === null) {
			await this.refresh();
		}

		if (this.node.children !== null) {
			for (const node of Object.values(this.node.children)) {
				yield new Node(this.filesystem, node);
			}
		}
	}

	async getChildren() {
		if (this.node.children === null) {
			await this.refresh();
		}
		return this.node.children;
	}

	async fetchContent(force = false) {
		if (this.content === null || force) {
			Exception.assert(this.isFile(), "Folder do not have content");
			this.content = await this.filesystem.api.request("get", "/file/content", {
				path: this.node.path,
			});
		}
	}

	async saveContent(content) {
		this.content = content;
		await this.filesystem.api.request("post", "/file/content", { path: this.node.path, content: this.content });
	}

	async updateContent(process) {
		await this.fetchContent();
		const cursor = await process(this);
		if (typeof cursor == "number") {
			this.setCursor(cursor);
		}
		await this.filesystem.api.request("post", "/file/content", { path: this.node.path, content: this.content });
	}

	async toggleExpand() {
		Exception.assert(this.isFolder(), "Only folders can be expanded");
		this.node.expanded = !this.node.expanded;
	}

	/// Set the new position for the cursor
	setCursor(position) {
		this.node.cursor = position;
	}

	/// Checks if the node is a directory
	isFolder() {
		return FileSystem.isFolder(this.node);
	}

	/// Checks if the node is a file
	isFile() {
		return FileSystem.isFile(this.node);
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
		this.tree = { path: "/", children: null };
		// Selected node if any.
		this.selected = null;
	}

	/// Create a renderable tree data.
	get node() {
		return new Node(this, this.tree);
	}

	makeRootNode(name) {
		return new Node(this, {
			children: {
				[name]: Object.assign({ name: name, expanded: true }, this.tree),
			},
		});
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
		let item = await this.select(path, /*select*/ false);
		Exception.assert(item.isFolder(), "The file to be listed must be a folder: {}", path);
		let node = item.node;

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
					data["children"] = null;
				}
				Vue.set(obj, item.name, data);
				return obj;
			}, {}),
		);
	}

	/// Create a new file at a given path.
	async createFile(path, initialName = null) {
		const dirname = FileSystem.dirname(path);
		const basename = FileSystem.basename(path);
		const current = await this.createFolder(dirname);
		let children = await current.getChildren();
		Vue.set(children, basename, { name: initialName === null ? basename : initialName, path: path });
		await this.api.request("post", "/file/content", { path: path });
		this.selected = this.makeNode_(children[basename]);
	}

	/// Create a new folder at a given path.
	async createFolder(path) {
		let current = this.makeNode_(this.tree);
		let currentPathList = [];
		for (const name of FileSystem.toPathList(path)) {
			let children = await current.getChildren();
			currentPathList.push(name);
			if (name in children) {
				Exception.assert(FileSystem.isFolder(children[name]), "Folder path conflicts with an existing file: {}", path);
			} else {
				Vue.set(children, name, { name: name, path: currentPathList.join("/"), expanded: true, children: {} });
			}
			current = this.makeNode_(children[name]);
		}
		this.selected = current;
		return current;
	}

	/// Set file permission.
	async setExecutable(path) {
		Exception.assert(await this.select(path), "The file must exists");
		await this.api.request("post", "/file/executable", { path: path });
	}

	/// Get a node refered at a specified path.
	async select(path, select = true) {
		let current = this.makeNode_(this.tree);
		for (const name of FileSystem.toPathList(path)) {
			const children = await current.getChildren();
			if (!(name in children)) {
				return null;
			}
			current = this.makeNode_(children[name]);
		}
		if (select) {
			// Do not re-select the file if it is already.
			if (!this.selected || this.selected.node !== current.node) {
				this.selected = current;
			}
			current = this.selected;
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

	/// Refresh the tree.
	refresh() {
		this.tree.children = null;
	}

	makeNode_(node) {
		return new Node(this, node);
	}
}
