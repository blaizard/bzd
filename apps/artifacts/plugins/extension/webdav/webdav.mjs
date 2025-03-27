import { XMLParser, XMLBuilder } from "fast-xml-parser";
import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";

export default function extensionWebdav(plugin, options, provider, endpoints) {
	const optionsWebdav = options["webdav"] || null;
	if (!optionsWebdav) {
		return;
	}

	endpoints.register("get", "/webdav/{path:*}", async (context) => {
		console.log("RECEIVED!!!!");
		const body = context.getBody();
		const storage = plugin.getStorage();

		context.redirect("/file/test/" + context.getParam("path"));
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
		const body = context.getBody();
		const storage = plugin.getStorage();

		if (depth != "0" && depth != "1") {
			context.sendStatus(403);
			return;
		}

		const entry = await storage.metadata(path.parts);
		const entryToResponse = (path, entry) => {
			let props = {
				"D:displayname": entry.name || path.name,
				"D:resourcetype": Permissions.makeFromEntry(entry).isList() ? { "D:collection": {} } : {},
			};
			if ("size" in entry) {
				props["D:getcontentlength"] = entry.size;
			}
			return {
				"$xmlns:D": "DAV:",
				"D:href": "/" + path.asPosix(),
				"D:propstat": {
					"D:prop": props,
				},
				"D:status": "HTTP/1.1 200 OK",
			};
		};

		let responses = [entryToResponse(path, entry)];
		if (depth == "1") {
			const children = await storage.list(path.parts, 100, /*includeMetadata*/ true);
			responses = responses.concat(children.map((entry) => entryToResponse(path.join(entry.name).parts, entry)));
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
