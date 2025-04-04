import { XMLBuilder } from "fast-xml-parser";
import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";
import ExceptionFactory from "../../../../../nodejs/core/exception.mjs";
import { FileNotFoundError } from "#bzd/nodejs/db/storage/storage.mjs";

const Exception = ExceptionFactory("apps", "plugin", "webdav");

/// Simple exception handler.
async function scopeExceptionHandler(context, action) {
	try {
		await action();
	} catch (e) {
		if (e instanceof FileNotFoundError) {
			context.sendStatus(404);
			return;
		}
		throw e;
	}
}

/// Set the headers of a specific resource.
async function setRessourceHeaders(pathList, context, storage) {
	const metadata = await storage.metadata(pathList);
	if (metadata.size) {
		context.setHeader("Content-Length", metadata.size);
	}
	if (metadata.modified) {
		context.setHeader("Last-Modified", metadata.modified.toUTCString());
	}
	context.setHeader("Content-Disposition", 'attachment; filename="' + metadata.name + '"');
	context.setHeader("Content-Type", "application/octet-stream");
}

export default function extensionWebdav(plugin, options, provider, endpoints) {
	if (!options["webdav"]) {
		return;
	}

	const optionsWebdav = Object.assign(
		{
			/// Download and upload timeout in seconds.
			timeoutS: 10 * 60,
			/// Upload limit in bytes.
			limit: 20 * 1024 * 1024, // 20 MB
		},
		options["webdav"],
	);

	/// Write the content of a file.
	endpoints.register("options", "/webdav/{path:*}", async (context) => {
		context.setHeader("Allow", "GET, PUT, DELETE, OPTIONS, PROPFIND, HEAD");
		context.setHeader("Content-Type", "httpd/unix-directory");
		context.sendStatus(200);
	});

	/// Read the content of a file.
	endpoints.register(
		"get",
		"/webdav/{path:*}",
		async (context) => {
			await scopeExceptionHandler(context, async () => {
				const path = pathlib.path(context.getParam("path", "")).normalize;
				const storage = plugin.getStorage();
				await setRessourceHeaders(path.parts, context, storage);
				context.setStatus(200);
				const stream = await storage.read(path.parts);
				await context.sendStream(stream);
			});
		},
		{
			timeoutS: optionsWebdav.timeoutS,
		},
	);

	/// Write the content of a file.
	endpoints.register(
		"put",
		"/webdav/{path:*}",
		async (context) => {
			await scopeExceptionHandler(context, async () => {
				const path = pathlib.path(context.getParam("path", "")).normalize;
				const storage = plugin.getStorage();
				const body = context.getBody();
				await storage.writeFromChunk(path.parts, body);
				context.sendStatus(200);
			});
		},
		{
			limit: optionsWebdav.limit,
			timeoutS: optionsWebdav.timeoutS,
		},
	);

	// See: https://learn.microsoft.com/en-us/previous-versions/office/developer/exchange-server-2003/aa142960(v=exchg.65)
	//      https://github.com/nfarina/simpledav/blob/master/views.py
	/// Get the properties of a resource.
	///
	/// Possible body:
	/// <?xml version="1.0"?>
	/// <a:propfind xmlns:a="DAV:">
	///		<a:prop><a:getcontenttype/></a:prop>
	///		<a:prop><a:getcontentlength/></a:prop>
	/// </a:propfind>
	endpoints.register("propfind", "/webdav/{path:*}", async (context) => {
		await scopeExceptionHandler(context, async () => {
			const depth = context.getHeader("depth", "0");
			const path = pathlib.path(context.getParam("path", "")).normalize;
			const storage = plugin.getStorage();

			if (depth != "0" && depth != "1") {
				context.sendStatus(403);
				return;
			}

			const entryToResponse = (path, entry) => {
				let props = {
					"D:displayname": entry.name || path.name,
					"D:resourcetype": Permissions.makeFromEntry(entry).isList() ? { "D:collection": {} } : {},
				};
				if (entry.size) {
					props["D:getcontentlength"] = entry.size;
				}
				if (entry.modified) {
					props["D:getlastmodified"] = entry.modified.toUTCString();
				}
				if (entry.created) {
					props["D:creationdate"] = entry.created.toUTCString();
				}
				return {
					"$xmlns:D": "DAV:",
					"D:href": context.getHost() + path.asPosix(),
					"D:propstat": {
						"D:prop": props,
					},
					"D:status": "HTTP/1.1 200 OK",
				};
			};

			const publicPath = pathlib.path(context.getPath()).normalize;
			Exception.assert(publicPath.isAbsolute(), "The public path must be absolute: {}", publicPath.asPosix());

			const entry = await storage.metadata(path.parts);

			let responses = [entryToResponse(publicPath, entry)];
			if (depth == "1") {
				try {
					const children = await storage.list(path.parts, 100, /*includeMetadata*/ true);
					responses = responses.concat(
						children.data().map((entry) => entryToResponse(publicPath.joinPath(entry.name), entry)),
					);
				} catch (e) {
					// ignore errors, it means that the list was performed on a file most likely.
				}
			}

			const builder = new XMLBuilder({
				ignoreAttributes: false,
				attributeNamePrefix: "$",
			});
			const xml = builder.build({
				multistatus: {
					$xmlns: "DAV:",
					"D:response": responses,
				},
			});
			context.setHeader("Content-Type", 'text/xml; charset="utf-8"');
			context.setStatus(207);
			context.send(xml);
		});
	});

	/// Delete a file/directory.
	endpoints.register("delete", "/webdav/{path:*}", async (context) => {
		await scopeExceptionHandler(context, async () => {
			const path = pathlib.path(context.getParam("path", "")).normalize;
			const storage = plugin.getStorage();

			await storage.delete(path.parts);
			context.sendStatus(200);
		});
	});

	/// Get the header only to query a resource.
	endpoints.register("head", "/webdav/{path:*}", async (context) => {
		await scopeExceptionHandler(context, async () => {
			const path = pathlib.path(context.getParam("path", "")).normalize;
			const storage = plugin.getStorage();
			await setRessourceHeaders(path.parts, context, storage);
			context.sendStatus(200);
		});
	});
}
