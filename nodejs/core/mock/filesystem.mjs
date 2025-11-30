import Pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import ClockDate from "#bzd/nodejs/core/clock/date.mjs";

const Exception = ExceptionFactory("filesystem");

class Entry {
	constructor(clock) {
		const timeS = clock.getTimeS();
		this.ctimeS = timeS;
		this.atimeS = timeS;
		this.mtimeS = timeS;
		this.read = true;
		this.write = true;
		this.exec = false;
	}
}

class File extends Entry {
	constructor(clock) {
		super(clock);
		this.content = "";
	}
}

class Symlink extends Entry {
	constructor(path, clock) {
		super(clock);
		this.path = path;
	}
}

class Directory extends Entry {
	constructor(clock) {
		super(clock);
		// List of children entries.
		this.children = {};
	}
}

class Dirent {
	constructor(name, entry) {
		this._name = name;
		this.entry = entry;
	}
	isBlockDevice() {
		return false;
	}
	isCharacterDevice() {
		return false;
	}
	isDirectory() {
		return this.entry instanceof Directory;
	}
	isFIFO() {
		return false;
	}
	isFile() {
		return this.entry instanceof File;
	}
	isSocket() {
		return false;
	}
	isSymbolicLink() {
		return this.entry instanceof Symlink;
	}
	get name() {
		return this._name;
	}
}

const mode = {
	S_IFMT: 61440, // bit mask for the file type bit fields
	S_IFSOCK: 49152, // socket
	S_IFLNK: 40960, // symbolic link
	S_IFREG: 32768, // regular file
	S_IFBLK: 24576, // block device
	S_IFDIR: 16384, // directory
	S_IFCHR: 8192, // character device
	S_IFIFO: 4096, // FIFO
	S_ISUID: 2048, // set UID bit
	S_ISGID: 1024, // set-group-ID bit (see below)
	S_ISVTX: 512, // sticky bit (see below)
	S_IRWXU: 448, // mask for file owner permissions
	S_IRUSR: 256, // owner has read permission
	S_IWUSR: 128, // owner has write permission
	S_IXUSR: 64, // owner has execute permission
	S_IRWXG: 56, // mask for group permissions
	S_IRGRP: 32, // group has read permission
	S_IWGRP: 16, // group has write permission
	S_IXGRP: 8, // group has execute permission
	S_IRWXO: 7, // mask for permissions for others (not in group)
	S_IROTH: 4, // others have read permission
	S_IWOTH: 2, // others have write permission
	S_IXOTH: 1, // others have execute permission
};

/// Mock for class fs.Stats
class Stats {
	constructor(entry) {
		this.entry = entry;
	}
	isBlockDevice() {
		return false;
	}
	isCharacterDevice() {
		return false;
	}
	isDirectory() {
		return this.entry.constructor == Object;
	}
	isFIFO() {
		return false;
	}
	isFile() {
		return this.entry instanceof File;
	}
	isSocket() {
		return false;
	}
	isSymbolicLink() {
		return this.entry instanceof Symlink;
	}
	get dev() {
		return 42;
	}
	get ino() {
		return 1337;
	}
	get mode() {
		let access = 0;
		if (this.isFile() || this.isSymbolicLink()) {
			access =
				(this.entry.read ? mode.S_IRUSR + mode.S_IRGRP + mode.S_IROTH : 0) +
				(this.entry.write ? mode.S_IWUSR + mode.S_IWGRP + mode.S_IWOTH : 0) +
				(this.entry.exec ? mode.S_IXUSR + mode.S_IXGRP + mode.S_IXOTH : 0);
		} else if (this.isDirectory()) {
			access = mode.S_IRUSR + mode.S_IRGRP + mode.S_IROTH;
		}
		return (
			(this.isSocket() ? mode.S_IFSOCK : 0) +
			(this.isSymbolicLink() ? mode.S_IFLNK : 0) +
			(this.isFile() ? mode.S_IFREG : 0) +
			(this.isBlockDevice() ? mode.S_IFBLK : 0) +
			(this.isDirectory() ? mode.S_IFDIR : 0) +
			(this.isCharacterDevice() ? mode.S_IFCHR : 0) +
			(this.isFIFO() ? mode.S_IFIFO : 0) +
			access
		);
	}
	get nlink() {
		return 0;
	}
	get uid() {
		return 132;
	}
	get gid() {
		return 434;
	}
	get rdev() {
		return 3;
	}
	get size() {
		return this.isFile() ? this.entry.content.length : 0;
	}
	get blksize() {
		return 1024;
	}
	get blocks() {
		return this.size / this.blksize + 1;
	}
	get atimeMs() {
		return this.entry.atimeS * 1000;
	}
	get mtimeMs() {
		return this.entry.mtimeS * 1000;
	}
	get ctimeMs() {
		return this.entry.ctimeS * 1000;
	}
	get atime() {
		return new Date(this.mtimeMs);
	}
	get mtime() {
		return new Date(this.atimeMs);
	}
	get ctime() {
		return new Date(this.ctimeMs);
	}
}

/// Policy to follow when navigating in the directory tree.
const _Policy = Object.freeze({
	mustExists: 0b00001,
	create: 0b00010,
	file: 0b00100,
	directory: 0b01000,
	touch: 0b10000,
});

export default class FileSystem {
	/// Create a mocked filesystem.
	///
	/// \param init dictionary to initialize the root folder, with keys corresponding to the path,
	///             and the value, the content of the file.
	constructor(init = {}, options = {}) {
		this.options = Object.assign(
			{
				clock: new ClockDate(),
			},
			options,
		);
		this.root = new Directory(this.options.clock);
		for (const [path, content] of Object.entries(init)) {
			const file = this._toEntry(path, _Policy.create | _Policy.file);
			file.content = content;
		}
	}

	static async _mockWait(ms = null) {
		if (ms === null) {
			// Generates 0 or 1.
			ms = Math.round(Math.random());
		}
		return new Promise((resolve) => {
			setTimeout(resolve, ms);
		});
	}

	/// Return the entry at a given path.
	_toEntry(path, policy) {
		const parts = Pathlib.path(path).normalize.parts;
		let entry = this.root;

		for (const [index, segment] of parts.entries()) {
			const isLast = index == parts.length - 1;
			Exception.assert(
				entry instanceof Directory,
				"The entry must be a directory '{}' (directory '{}').",
				path,
				segment,
			);

			// If the entry does not exists.
			if (!(segment in entry.children)) {
				Exception.assert(policy & _Policy.create, "The path '{}' does not exists (directory '{}').", path, segment);
				if (isLast && policy & _Policy.file) {
					entry.children[segment] = new File(this.options.clock);
				} else {
					entry.children[segment] = new Directory(this.options.clock);
				}
			}
			entry = entry.children[segment];

			// Resolve symlinks.
			if (entry instanceof Symlink) {
				entry = this._toEntry(entry.path, _Policy.mustExists);
			}

			if (entry instanceof File) {
				Exception.assert(isLast, "A file exists in the path '{}'.", path);
				Exception.assert(!(policy & _Policy.directory), "The path '{}' points to a file.", path);
			} else if (entry instanceof Directory) {
				if (isLast) {
					Exception.assert(!(policy & _Policy.file), "The path '{}' points to a directory.", path);
				}
			} else {
				Exception.unreachable("Invalid entry type for path '{}' ({:j}).", path, entry);
			}
		}

		// Update the timestamps.
		const accessTimeS = this.options.clock.getTimeS();
		entry.atimeS = accessTimeS;
		if (policy & _Policy.touch) {
			entry.mtimeS = accessTimeS;
		}

		return entry;
	}

	async mkdir(path, { force = true } = {}) {
		await FileSystem._mockWait();

		// Check the non-existence of the entry, this must be done atomically to mimic file systems.
		if (!force) {
			const p = Pathlib.path(path);
			const directory = this._toEntry(p.parent, _Policy.mustExists | _Policy.directory);
			Exception.assert(!(p.name in directory.children), "Directory '{}' already exists.", p.asPosix());
		}

		this._toEntry(path, _Policy.create | _Policy.directory);
	}

	/// Remove a directory recursively
	async rmdir(path, { force = false } = {}) {
		await FileSystem._mockWait();
		const p = Pathlib.path(path);
		const parent = this._toEntry(p.parent, _Policy.mustExists | _Policy.directory);
		if (!(p.name in parent.children)) {
			Exception.assert(force, "The file '{}' doesn't exists.", p.asPosix());
			return;
		}
		Exception.assert(parent.children[p.name] instanceof Directory, "The entry '{}' must be a directory.", p.asPosix());
		delete parent.children[p.name];
	}

	/// Read the content of a directory
	async readdir(path, withFileTypes = false) {
		await FileSystem._mockWait();
		const dir = this._toEntry(path, _Policy.mustExists | _Policy.directory);
		if (withFileTypes) {
			return Object.entries(dir.children).map(([name, entry]) => new Dirent(name, entry));
		}
		return Object.keys(dir.children);
	}

	/// Append data to a file
	async appendFile(path, data) {
		await FileSystem._mockWait();
		const file = this._toEntry(path, _Policy.create | _Policy.file | _Policy.touch);
		file.content += data;
	}

	/// Get the stat object associated with a file
	async stat(path) {
		await FileSystem._mockWait();
		const entry = this._toEntry(path, _Policy.mustExists);
		return new Stats(entry);
	}

	/// Delete a file
	async unlink(path) {
		await FileSystem._mockWait();
		const p = Pathlib.path(path);
		const dir = this._toEntry(p.parent, _Policy.mustExists | _Policy.directory);
		Exception.assert(p.name in dir.children, "The file '{}' doesn't exists.", p.asPosix());
		delete dir.children[p.name];
	}

	/// Touch a file (create it if it does not exists) and
	/// updates its last modification date.
	async touch(path) {
		await FileSystem._mockWait();
		this._toEntry(path, _Policy.create | _Policy.file | _Policy.touch);
	}

	/// Change the file system timestamps of the object referenced by path.
	async utimes(path, atimeS, mtimeS) {
		await FileSystem._mockWait();
		const file = this._toEntry(path, _Policy.mustExists | _Policy.file);
		file.atimeS = atimeS;
		file.mtimeS = mtimeS;
	}

	/// Read the content of a file
	async readFile(path, options = "utf8") {
		await FileSystem._mockWait();
		const file = this._toEntry(path, _Policy.mustExists | _Policy.file);
		return file.content;
	}

	/// Read the content of a binary file
	async readBinary(path) {
		await FileSystem._mockWait();
		const file = this._toEntry(path, _Policy.mustExists | _Policy.file);
		return new Buffer(file.content);
	}

	/// Write the content of a file
	async writeBinary(path, data) {
		await FileSystem._mockWait();
		const file = this._toEntry(path, _Policy.create | _Policy.file | _Policy.touch);
		file.content = data;
	}
}
