import { XMLParser, XMLBuilder } from "fast-xml-parser";
import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";
import ExceptionFactory from "../../../../../nodejs/core/exception.mjs";

const Exception = ExceptionFactory("apps", "plugin", "webdav");

export default function extensionWebdav(plugin, options, provider, endpoints) {
	const optionsWebdav = options["webdav"] || null;
	if (!optionsWebdav) {
		return;
	}

	endpoints.register("get", "/webdav/{path:*}", async (context) => {
		console.log("RECEIVED!!!!");
		const body = context.getBody();
		const storage = plugin.getStorage();

		context.redirect("/file/memory/" + context.getParam("path"));
	});

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
		const depth = context.getHeader("depth", "0");
		const path = pathlib.path(context.getParam("path")).normalize;
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
			const children = await storage.list(path.parts, 100, /*includeMetadata*/ true);
			responses = responses.concat(
				children.data().map((entry) => entryToResponse(publicPath.joinPath(entry.name), entry)),
			);
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
}
