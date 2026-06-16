import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import LoggerFactory from "#bzd/nodejs/core/log.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { FileNotFoundError } from "#bzd/nodejs/db/storage/storage.mjs";
import { HttpClient } from "#bzd/nodejs/core/http/client.mjs";
import {
	buildCacheEntryStream,
	CacheEntryReader,
} from "#bzd/apps/artifacts/plugins/extension/caching_proxy/cache_entry.mjs";
import { teeReadStream } from "#bzd/nodejs/core/stream.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";

import { createHash } from "crypto";

const Log = LoggerFactory("apps", "plugin", "caching-proxy");
const Exception = ExceptionFactory("apps", "plugin", "caching-proxy");

// Headers to be used to compute the cache key, they might have effect on the content.
const HEADERS_CACHE_KEY = ["accept-encoding", "accept", "accept-language"];
// Headers to remove from the request.
const HEADERS_REQUEST_BLACKLIST = new Set([
	"host",
	"connection",
	"keep-alive",
	"proxy-authenticate",
	"proxy-authorization",
	"trailer",
	"transfer-encoding",
	"upgrade",
	"range",
]);
// Headers to return in the response.
const HEADERS_RESPONSE = new Set([
	"content-length",
	"content-disposition",
	"content-type",
	"last-modified",
	"content-encoding",
	"etag",
]);
// Path to access the metadata of the files (it includes the access timestamp).
const PATH_GLOBAL_METADATA = "metadata.json";

function sanitizePathSegment(segment) {
	return segment
		.replace(/[^a-zA-Z0-9._-]+/g, "-")
		.replace(/^[-.]+/, "")
		.slice(0, 100);
}

function inputsToCacheKey(url, headers) {
	const toHash = [
		url.toString(),
		...HEADERS_CACHE_KEY.filter((name) => name in headers).map((name) => name + "=" + headers[name]),
	];
	return createHash("md5").update(toHash.join("\n")).digest("hex").slice(0, 32);
}

function inputsToPath(urlStr, headers) {
	try {
		const url = new URL(urlStr);
		const host = sanitizePathSegment(url.host);
		const segments = url.pathname.split("/").map(sanitizePathSegment).filter(Boolean);
		const cacheKey = inputsToCacheKey(url, headers);
		return [host, ...segments, cacheKey];
	} catch (e) {
		Exception.errorPrecondition("The url '{}' is malformed: {}", urlStr, e.message);
	}
}

function sendMetadata(context, metadata) {
	context.setStatus(metadata.status);
	for (const [name, value] of Object.entries(metadata.headers)) {
		context.setHeader(name, value);
	}
}

async function fetchFile(context, url, headers) {
	const headersRequest = Object.fromEntries(
		Object.entries(headers).filter(([headerName]) => {
			return !HEADERS_REQUEST_BLACKLIST.has(headerName);
		}),
	);
	const [readStream, headersResponse, code] = await HttpClient.get(url, {
		headers: headersRequest,
		expect: "stream",
		includeAll: true,
	});
	const metadata = {
		headers: Object.fromEntries(
			Object.entries(headersResponse).filter(([headerName]) => HEADERS_RESPONSE.has(headerName)),
		),
		status: code,
	};
	sendMetadata(context, metadata);
	return [metadata, readStream];
}

function sendErrorFromException(context, url, error) {
	const message = "Error while fetching " + url + ": " + error.message;
	try {
		context.sendStatus(500, message);
	} catch (e) {
		Log.warning("Response already sent to client; {}", message);
	}
}

async function readGlobalMetadata(storage) {
	try {
		const content = await storage.readToString([PATH_GLOBAL_METADATA]);
		return JSON.parse(content);
	} catch (e) {
		if (e instanceof FileNotFoundError) {
			return {};
		}
		throw e;
	}
}

export default function extensionCachingProxy(plugin, options, provider, endpoints) {
	if (!("cachingProxy" in options)) {
		return;
	}

	const maxStorageSize = options["cachingProxy"]["maxStorageSize"];

	// Contains update on the access metadata of all files.
	let globalMetadataUpdate = {};

	endpoints.register("get", "/@caching-proxy/{schema}/{path:*}", async (context) => {
		const schema = context.getParam("schema");
		const path = context.getParam("path", "");
		const url = schema + "://" + path;
		const headersRequest = context.getHeaders();
		const pathList = inputsToPath(url, headersRequest);
		const storage = plugin.getStorage();

		// Update the global metadata.
		globalMetadataUpdate[pathList.join("/")] = Date.now();

		try {
			const readStream = await storage.read(pathList);
			const transform = new CacheEntryReader((metadata) => {
				sendMetadata(context, metadata);
			});
			await context.sendStream(readStream, transform);
		} catch (e) {
			// If the file does not exists.
			if (e instanceof FileNotFoundError) {
				const [metadata, readStream] = await fetchFile(context, url, headersRequest);

				// Fetch a local copy and save it to the storage.
				const maybeLock = await plugin.locks.tryLock(pathList.join("/"));
				if (maybeLock) {
					Log.info("Fetching {}", url);
					const temporaryPath = pathList.with(-1, "." + pathList.at(-1) + ".temp");
					const [streamSend, streamWrite] = teeReadStream(readStream);
					const promiseSend = context.sendStream(streamSend);
					const promiseWrite = storage.write(
						temporaryPath,
						buildCacheEntryStream(metadata, streamWrite),
						/*mkdir*/ true,
					);
					try {
						await Promise.all([promiseSend, promiseWrite]);
						await storage.move(temporaryPath, pathList);
					} catch (e) {
						// Destroy all the pipes first.
						readStream.destroy();
						streamSend.destroy();
						streamWrite.destroy();
						// Prevent Unhandled Promise Rejections.
						await Promise.all([promiseSend.catch(() => {}), promiseWrite.catch(() => {})]);
						// Only after cleanup the the files and report the error.
						await storage.tryDelete(temporaryPath);
						await storage.tryDelete(pathList);
						sendErrorFromException(context, url, e);
					} finally {
						await maybeLock.unlock();
					}
				}
				// If the file is already being fetched.
				else {
					try {
						await context.sendStream(readStream);
					} catch (e) {
						sendErrorFromException(context, url, e);
					}
				}
			} else {
				sendErrorFromException(context, url, e);
			}
		}
	});

	// Add a garbage collector if needed.
	if (maxStorageSize) {
		provider.addTimeTriggeredProcess(
			"cache.garbage-collect",
			async () => {
				const storage = plugin.getStorage();
				let current = {};
				for await (const [path, entry] of storage.walk([], /*maxOrPaging*/ 100, /*includeMetadata*/ true)) {
					// Ignore top-level files, as these are the files that are used or metadata storing.
					if (!path.length) {
						continue;
					}
					// If it's a file.
					if (!Permissions.makeFromEntry(entry).isList()) {
						current[[...path, entry.name].join("/")] = entry.size;
					}
				}
				const totalSize = Object.values(current).reduce((sum, value) => sum + value, 0);

				// If it needs some cleanup.
				if (totalSize > maxStorageSize) {
					await plugin.locks.lock(PATH_GLOBAL_METADATA, async () => {
						const metadata = await readGlobalMetadata(storage);
						// TODO: implement cleanup.
						// Start with the entries that are in 'current' but not in 'metadata'.
						// Continue with the entries the oldest (from 'metadata').
					});
				}

				return {
					totalSize: totalSize,
				};
			},
			{
				periodS: 3600,
			},
		);
	}

	provider.addTimeTriggeredProcess(
		"cache.update-metadata",
		async () => {
			const nbEntries = Object.keys(globalMetadataUpdate).length;
			if (nbEntries > 0) {
				await plugin.locks.lock(PATH_GLOBAL_METADATA, async () => {
					const storage = plugin.getStorage();
					const metadata = await readGlobalMetadata(storage);
					for (const [path, timestamp] of Object.entries(globalMetadataUpdate)) {
						metadata[path] = timestamp;
					}
					const serializedMetadata = JSON.stringify(metadata);
					await storage.writeFromChunk([PATH_GLOBAL_METADATA], serializedMetadata);
				});
				globalMetadataUpdate = {};
			}
			return nbEntries;
		},
		{
			periodS: 60,
		},
	);
}
