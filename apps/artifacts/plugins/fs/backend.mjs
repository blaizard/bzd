import StorageDisk from "#bzd/nodejs/db/storage/disk.mjs";
import PluginBase from "#bzd/apps/artifacts/backend/plugin.mjs";
import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import format from "#bzd/nodejs/core/format.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";

/// Keep only the last n files from a directory.
async function keepLastFiles(storage, directory, maxFiles) {
	let files = [];
	for await (const [_, data] of CollectionPaging.makeIterator(async (maxOrPaging) => {
		return await storage.list(directory, maxOrPaging, /*includeMetadata*/ true);
	}, 50)) {
		files.push(data);
	}

	if (files.length > maxFiles) {
		// Sort by date
		files.sort((a, b) => a.modified - b.modified);
		const toDelete = files.slice(0, files.length - maxFiles);
		for (const file of toDelete) {
			await storage.delete([...directory, file.name]);
		}
	}
}

export default class Plugin extends PluginBase {
	constructor(volume, options, provider, endpoints) {
		super(volume, options);
		provider.addStartProcess(async () => {
			const storage = await StorageDisk.make(options["fs.root"], {
				mustExists: true,
			});
			this.setStorage(storage);
		});

		for (const [path, upload] of Object.entries(options["fs.upload"] || {})) {
			endpoints.register(
				"put",
				path + "/{path:*}",
				async (context) => {
					const body = context.getBody();
					const storage = this.getStorage();
					let fullPath = pathlib.path(path).joinPath(context.getParam("path"));

					// Rename the filename if needed.
					if ("rename" in upload) {
						const renamed = format(upload.rename, {
							name: fullPath.name,
							timestamp: Date.now(),
						});
						fullPath = fullPath.parent.joinPath(renamed);
					}

					await storage.writeFromChunk(fullPath.asPosix(), body);

					// If only the last X files should be kept.
					if ("rolling" in upload) {
						const directory = fullPath.parent.parts.slice(1);
						await keepLastFiles(storage, directory, upload.rolling);
					}
				},
				{
					limit: upload.limit || 20 * 1024 * 1024, // 20 MB
				},
			);
		}
	}
}
