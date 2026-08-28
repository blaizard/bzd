import Fs from "fs";
import Path from "path";

/// Collection of Promise-based asynchronous functions for the file system.
export default class FileSystem {
    /// Checks if the file or directory exists
    static async exists(path: string): Promise<boolean> {
        return new Promise((resolve) => {
            Fs.access(path, Fs.constants.F_OK, (e) => {
                resolve(e ? false : true);
            });
        });
    }

    /// Get the stat object associated with a file
    static async stat(path: string): Promise<Fs.Stats> {
        return await Fs.promises.stat(path);
    }

    /// Create a directory recursively.
    //
    // \param path The path of the directory.
    // \param force If false, it will throw if a directory already exists or if the parent path does not exists.
    static async mkdir(path: string, { force = true }: { force?: boolean } = {}): Promise<void> {
        await Fs.promises.mkdir(path, {
            recursive: force,
        });
    }

    /// Create a directory recursively, if it already exists, do nothing
    static mkdirSync(path: string, { force = true }: { force?: boolean } = {}): void {
        Fs.mkdirSync(path, {
            recursive: force,
        });
    }

    /// Remove a directory recursively.
    static async rmdir(path: string, { force = false }: { force?: boolean } = {}): Promise<void> {
        await Fs.promises.rm(path, { recursive: true, force: force });
    }

    /// Read the content of a file
    static async readFile(path: string, options: BufferEncoding = "utf8"): Promise<string> {
        const data = await Fs.promises.readFile(path, {
            encoding: options,
        });
        return data.toString();
    }

    /// Read a binary file as a buffer, do not decode any characters.
    static async readBinary(path: string): Promise<Buffer> {
        return await Fs.promises.readFile(path);
    }

    /// Write the content of a file
    static async writeFile(path: string, data: string, options: BufferEncoding = "utf8"): Promise<void> {
        await Fs.promises.writeFile(path, data, {
            encoding: options,
        });
    }

    /// Write the content of a file as binary, do not attempt any encoding.
    static async writeBinary(path: string, data: string | Buffer): Promise<void> {
        await Fs.promises.writeFile(path, data);
    }

    /// Read the content of a directory
    static async readdir(path: string, withFileTypes: boolean = false): Promise<(string | Fs.Dirent)[]> {
        if (withFileTypes) {
            return await Fs.promises.readdir(path, { withFileTypes: true });
        }
        return await Fs.promises.readdir(path);
    }

    /// Move a file from a location to another. Note pathFrom and pathTo
    /// are the full path of the file including the file name.
    static async move(pathFrom: string, pathTo: string): Promise<void> {
        try {
            await Fs.promises.rename(pathFrom, pathTo);
        } catch (error) {
            if ((error as NodeJS.ErrnoException).code === "EXDEV") {
                await Fs.promises.copyFile(pathFrom, pathTo);
                await Fs.promises.unlink(pathFrom);
            } else {
                throw error;
            }
        }
    }

    /// Rename a file.
    static async rename(pathFrom: string, name: string): Promise<void> {
        const pathTo = Path.join(Path.dirname(pathFrom), name);
        await Fs.promises.rename(pathFrom, pathTo);
    }

    /// Copy a file from a location to another. Note pathFrom and pathTo
    /// are the full path of the file including the file name.
    /// If the destination file already exists, it will fail.
    static async copy(pathFrom: string, pathTo: string): Promise<void> {
        await Fs.promises.copyFile(pathFrom, pathTo, Fs.constants.COPYFILE_EXCL);
    }

    /// Delete a file
    static async unlink(path: string): Promise<void> {
        await Fs.promises.unlink(path);
    }

    /// Changes the permissions of a file.
    static async chmod(path: string, mode: number): Promise<void> {
        await Fs.promises.chmod(path, mode);
    }

    /// Append data to a file
    static async appendFile(path: string, data: string | Buffer): Promise<void> {
        await Fs.promises.appendFile(path, data);
    }

    /// Truncate a file to a specific size
    static async truncate(path: string, fileSize: number): Promise<void> {
        await Fs.promises.truncate(path, fileSize);
    }

    /// Touch a file (create it if it does not exists) and
    /// updates its last modification date.
    static async touch(path: string): Promise<void> {
        const fd = await FileSystem.open(path, "a");
        await FileSystem.close(fd);
    }

    /// Change the file system timestamps of the object referenced by path.
    static async utimes(path: string, atimeS: number, mtimeS: number): Promise<void> {
        await Fs.promises.utimes(path, atimeS, mtimeS);
    }

    /// Open a file asynchronously
    static async open(path: string, options: Fs.OpenMode): Promise<Fs.promises.FileHandle> {
        return await Fs.promises.open(path, options);
    }

    /// Close a file asynchronously, previously open with "open"
    static async close(fileHandle: Fs.promises.FileHandle): Promise<void> {
        await fileHandle.close();
    }

    /// Determines the actual location of a path and resolve symlinks.
    static async realpath(path: string): Promise<string> {
        return await Fs.promises.realpath(path);
    }

    /// Creates a symbolic link.
    static async symlink(target: string, path: string): Promise<void> {
        return await Fs.promises.symlink(target, path);
    }

    /// Creates a hard link.
    static async hardlink(target: string, path: string): Promise<void> {
        return await Fs.promises.link(target, path);
    }

    /// Walk through a directory and print all files
    static async *walk(path: string): AsyncGenerator<string> {
        const dirents = await Fs.promises.readdir(path, { withFileTypes: true });
        for (const dirent of dirents) {
            const res = Path.resolve(path, dirent.name);
            if (dirent.isDirectory()) {
                yield* FileSystem.walk(res);
            } else {
                yield res;
            }
        }
    }
}
