import StorageDisk from "#bzd/nodejs/db/storage/disk.mjs";
import PluginBase from "#bzd/apps/artifacts/backend/plugin.mjs";
import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import format from "#bzd/nodejs/core/format.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";

/// The FileSystem plugin support the following features:
///
/// - upload:
///   Allow certain directory to be uploadable, this also comes with several options.
/// - release:
///   Allow certain directory to be used as release depot. This creates an endpoint to get a release if available.
export default class Plugin extends PluginBase {
	constructor(volume, options, provider, endpoints) {
		super(volume, options);
		provider.addStartProcess(async () => {
			const storage = await StorageDisk.make(options["fs.root"], {
				mustExists: true,
			});
			this.setStorage(storage);
		});

		// Set upload endpoints
		for (const [path, uploadOptions] of Object.entries(options["fs.upload"] || {})) {
			endpoints.register(
				"put",
				path + "/{path:*}",
				async (context) => {
					const body = context.getBody();
					const storage = this.getStorage();
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
						await this.keepLastFiles(storage, directory, uploadOptions.rolling);
					}

					context.sendStatus(200);
				},
				{
					limit: uploadOptions.limit || 20 * 1024 * 1024, // 20 MB
				},
			);
		}

		// Set release endpoints
		for (const [path, _] of Object.entries(options["fs.release"] || {})) {
			endpoints.register("get", path + "/{path:*}", async (context) => {
				const storage = this.getStorage();
				const directory = pathlib.path(path).joinPath(context.getParam("path")).parts.slice(1);
				const files = await this.listAllFiles(storage, directory);
				console.log(files);
				const maybePreviousFile = context.getQuery("after");
				console.log(context.getQuery("uid"));
				if (files.length) {
					const file = files[0];
					const fileName = file.name;
					const stream = await storage.read([...directory, fileName]);
					context.setHeader("Content-Length", file.size);
					context.setHeader("Content-Disposition", 'attachment; filename="' + fileName + '"');
					context.setHeader("Last-Modified", file.modified.toUTCString());
					await context.sendStream(stream);
					context.sendStatus(200);
				} else {
					context.sendStatus(204);
				}
			});
		}
	}

	/// List all files from a directory.
	async listAllFiles(storage, directory) {
		let files = [];
		for await (const [_, data] of CollectionPaging.makeIterator(async (maxOrPaging) => {
			return await storage.list(directory, maxOrPaging, /*includeMetadata*/ true);
		}, 50)) {
			files.push(data);
		}
		return files;
	}

	/// Keep only the last n files from a directory.
	async keepLastFiles(storage, directory, maxFiles) {
		const files = await this.listAllFiles(storage, directory);

		if (files.length > maxFiles) {
			// Sort by date
			files.sort((a, b) => a.modified - b.modified);
			const toDelete = files.slice(0, files.length - maxFiles);
			for (const file of toDelete) {
				await storage.delete([...directory, file.name]);
			}
		}
	}
}
