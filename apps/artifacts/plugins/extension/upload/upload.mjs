import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import format from "#bzd/nodejs/core/format.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";

export default function extensionUpload(plugin, options, provider, endpoints) {
	for (const [path, uploadOptions] of Object.entries(options["upload"] || {})) {
		endpoints.register(
			"put",
			path + "/{path:*}",
			async (context) => {
				const body = context.getBody();
				const storage = plugin.getStorage();
				let fullPath = pathlib.path(path).joinPath(context.getParam("path"));

				// Rename the filename if needed.
				if ("rename" in uploadOptions) {
					const renamed = format(uploadOptions.rename, {
						name: fullPath.name,
						timestamp: Date.now(),
					});
					fullPath = fullPath.parent.joinPath(renamed);
				}

				await storage.writeFromChunk(fullPath.asPosix(), body);

				// If only the last X files should be kept.
				if ("rolling" in uploadOptions) {
					const directory = fullPath.parent.parts.slice(1);
					await keepLastFiles(storage, directory, uploadOptions.rolling);
				}

				context.sendStatus(200);
			},
			{
				limit: uploadOptions.limit || 20 * 1024 * 1024, // 20 MB
				timeoutS: 30 * 60, // 30 min
			},
		);
	}
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

/// Keep only the last n files from a directory.
async function keepLastFiles(storage, directory, maxFiles) {
	const files = await listAllFiles(storage, directory);

	if (files.length > maxFiles) {
		// Sort by date
		files.sort((a, b) => a.modified - b.modified);
		const toDelete = files.slice(0, files.length - maxFiles);
		for (const file of toDelete) {
			await storage.delete([...directory, file.name]);
		}
	}
}
