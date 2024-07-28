export default class Permissions {
	constructor(permissions = {}) {
		this.permissions = permissions;
	}

	static makeFromString(permissions) {
		return new Permissions({
			read: permissions.includes("r"),
			write: permissions.includes("w"),
			delete: permissions.includes("d"),
			list: permissions.includes("l"),
			executable: permissions.includes("x"),
		});
	}

	static makeFromEntry(entry) {
		const permissionsStr = entry.permissions || "";
		return Permissions.makeFromString(permissionsStr);
	}

	/// Helper to add capabilities to entries.
	static makeEntry(entry, permissions) {
		entry.permissions = new Permissions(permissions).encode();
		return entry;
	}

	encode() {
		return (
			(this.permissions.read ? "r" : "") +
			(this.permissions.write ? "w" : "") +
			(this.permissions.delete ? "d" : "") +
			(this.permissions.list ? "l" : "") +
			(this.permissions.executable ? "x" : "")
		);
	}

	isRead() {
		return this.permissions.read || false;
	}

	isWrite() {
		return this.permissions.write || false;
	}

	isDelete() {
		return this.permissions.delete || false;
	}

	isList() {
		return this.permissions.list || false;
	}

	isExecutable() {
		return this.permissions.executable || false;
	}

	toString() {
		return this.encode();
	}
}
