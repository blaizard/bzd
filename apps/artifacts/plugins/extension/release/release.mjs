import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";

export default function extensionRelease(plugin, options, provider, endpoints) {
	// Set release endpoints
	for (const [path, _] of Object.entries(options["release"] || {})) {
		endpoints.register("get", path + "/{path:*}", async (context) => {
			const storage = plugin.getStorage();
			const root = pathlib.path(path).joinPath(context.getParam("path")).parts.slice(1);
			const directory = await getDirectoryFromPlatform(storage, root, context.getQuery("al"), context.getQuery("isa"));
			const files = await listAllFiles(storage, directory);
			const maybeFile = getFile(files, context);

			if (maybeFile) {
				const stream = await storage.read([...directory, maybeFile.name]);
				context.setHeader("Content-Disposition", 'attachment; filename="' + maybeFile.name + '"');
				if (maybeFile.size) {
					context.setHeader("Content-Length", maybeFile.size);
				}
				if (maybeFile.modified && maybeFile.modified instanceof Date) {
					context.setHeader("Last-Modified", maybeFile.modified.toUTCString());
				}
				await context.sendStream(stream);
				context.sendStatus(200);
			} else {
				context.sendStatus(204);
			}
		});
	}
}

/// Get the file expected to be sent from a list of files.
function getFile(files, context) {
	if (files.length == 0) {
		return null;
	}

	// Sort alphabetcally in reverse order to have the latest file first.
	const filesSorted = files.sort((a, b) => {
		if (a.name > b.name) {
			return -1;
		}
		if (a.name < b.name) {
			return 1;
		}
		return 0;
	});
	const file = filesSorted[0];
	const fileName = file.name;

	// If 'after' is provided, check if the file is not this one.
	const maybePreviousFile = context.getQuery("after");
	if (maybePreviousFile) {
		if (fileName == pathlib.path(maybePreviousFile).name) {
			return null;
		}
	}

	return file;
}

/// Get the directory given the platform.
async function getDirectoryFromPlatform(storage, root, al, isa) {
	// Check if a subdirectory matches the platform
	const directories = await listAllDirectories(storage, root);
	if (directories.length > 0) {
		// Build a map of platform segments to directory name.
		let mapPlatformDirectory = {};
		for (const directory of directories) {
			const normalizedDirectoryName = directory.name.toLowerCase();
			const segments = [...normalizedDirectoryName.split("-"), ...normalizedDirectoryName.split("_")];
			for (const segment in segments) {
				mapPlatformDirectory[segment] ??= new Set();
				mapPlatformDirectory[segment].add(directory.name);
			}
		}

		// Assess which directory is the most relevant.
		const alSet = mapPlatformDirectory[al] || new Set();
		const isaSet = mapPlatformDirectory[isa] || new Set();
		const mapOccurences = [...alSet, ...isaSet].reduce((counter, key) => {
			counter[key] = 1 + counter[key] || 1;
			return counter;
		}, {});
		const occurenceSorted = Object.entries(mapOccurences).sort((a, b) => b[1] - a[1]);
		if (occurenceSorted.length > 0) {
			return [...root, occurenceSorted[0][0]];
		}
	}
	return root;
}

/// List all directories from a directory.
async function listAllDirectories(storage, directory) {
	let files = [];
	for await (const [_, data] of CollectionPaging.makeIterator(async (maxOrPaging) => {
		return await storage.list(directory, maxOrPaging, /*includeMetadata*/ true);
	}, 50)) {
		const permissions = Permissions.makeFromEntry(data);
		if (permissions.isList()) {
			files.push(data);
		}
	}
	return files;
}

/// List all files from a directory.
async function listAllFiles(storage, directory) {
	let files = [];
	for await (const [_, data] of CollectionPaging.makeIterator(async (maxOrPaging) => {
		return await storage.list(directory, maxOrPaging, /*includeMetadata*/ true);
	}, 50)) {
		const permissions = Permissions.makeFromEntry(data);
		if (!permissions.isList()) {
			files.push(data);
		}
	}
	return files;
}
