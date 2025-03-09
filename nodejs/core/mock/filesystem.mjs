import Pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("filesystem");

class FileBase {
	constructor() {
		this.read = true;
		this.write = true;
		this.exec = false;
	}
}

class File extends FileBase {
	constructor() {
		super();
		this.content = "";
	}
}

class Symlink extends FileBase {
	constructor(path) {
		super();
		this.path = path;
	}
}

class Dirent {
	constructor(name, entry) {
		this.name = name;
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
	get name() {
		return this.name;
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
}

export default class FileSystem {
	/// Create a mocked filesystem.
	///
	/// \param init dictionary to initialize the root folder, with keys corresponding to the path,
	///             and the value, the content of the file.
	constructor(init = {}) {
		this.root = {};
		for (const [path, content] of Object.entries(init)) {
			const file = this._toFile(path, /*mustExists*/ false);
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

	/// Return the directory dictionary at a given path.
	///
	/// \return Will always return a directory dictionary.
	_toDir(path, mustExists) {
		const p = Pathlib.path(path);
		let dir = this.root;
		for (const segment of p.normalize.parts) {
			if (!(segment in dir)) {
				Exception.assert(!mustExists, "The path '{}' does not exists (directory '{}').", p.asPosix(), segment);
				dir[segment] = {};
			}
			dir = dir[segment];
			// Resolve the symlink.
			if (dir instanceof Symlink) {
				dir = this._toDir(dir.path, /*mustExists*/ true);
			}
			Exception.assert(dir.constructor == Object, "A file exists in the path '{}'.", p.asPosix());
		}
		return dir;
	}

	/// Touch a file and return the file object.
	_toFile(path, mustExists) {
		const p = Pathlib.path(path);
		const dir = this._toDir(p.parent, /*mustExists*/ false);
		const name = p.name;
		if (name in dir) {
			if (dir[name] instanceof Symlink) {
				return this._toFile(dir[name].path, /*mustExists*/ true);
			}
			Exception.assert(dir[name] instanceof File, "The path '{}' is pointing toward a directory.", p.asPosix());
			return dir[name];
		}
		Exception.assert(mustExists == false, "The file '{}' doesn't exists.", p.asPosix());
		dir[name] = new File();
		return dir[name];
	}

	async mkdir(path) {
		await FileSystem._mockWait();
		this._toDir(path, /*mustExists*/ false);
	}

	/// Read the content of a directory
	async readdir(path, withFileTypes = false) {
		await FileSystem._mockWait();
		const dir = this._toDir(path, /*mustExists*/ true);
		if (withFileTypes) {
			return Object.entries(dir).map(([name, entry]) => new Dirent(name, entry));
		}
		return Object.keys(dir);
	}

	/// Append data to a file
	async appendFile(path, data) {
		await FileSystem._mockWait();
		const file = this._toFile(path, /*mustExists*/ false);
		file.content += data;
	}

	/// Get the stat object associated with a file
	async stat(path) {
		await FileSystem._mockWait();
		const file = this._toFile(path, /*mustExists*/ true);
		return new Stats(file);
	}

	/// Delete a file
	async unlink(path) {
		await FileSystem._mockWait();
		const p = Pathlib.path(path);
		const dir = this._toDir(p.parent, /*mustExists*/ true);
		Exception.assert(p.name in dir, "The file '{}' doesn't exists.", p.asPosix());
		delete dir[p.name];
	}
}
