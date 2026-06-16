import { Readable } from "stream";
import { buffer } from "stream/consumers";
import {
	buildCacheEntryStream,
	CacheEntryReader,
} from "#bzd/apps/artifacts/plugins/extension/caching_proxy/cache_entry.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("test", "cache_entry");

async function readCacheEntry(chunks) {
	let metadata = null;
	const reader = new CacheEntryReader((meta) => {
		metadata = meta;
	});
	const bodyPromise = buffer(reader);
	for (const chunk of chunks) {
		reader.write(chunk);
	}
	reader.end();
	const body = await bodyPromise;
	return { metadata, body };
}

describe("CacheEntry", () => {
	it("empty body stream", async () => {
		const stream = buildCacheEntryStream({ key: "value" }, Readable.from([]));
		const fullBuffer = await buffer(stream);
		const { metadata, body } = await readCacheEntry([fullBuffer]);

		Exception.assertEqual(metadata, { key: "value" });
		Exception.assertEqual(body.length, 0);
	});

	it("truncated in length phase", async () => {
		await Exception.assertThrowsWithMatch(async () => {
			await readCacheEntry([Buffer.from([0x00, 0x00])]);
		}, "Truncated cache entry");
	});

	it("truncated in meta phase", async () => {
		const lenBuf = Buffer.alloc(4);
		lenBuf.writeUInt32BE(100, 0);
		await Exception.assertThrowsWithMatch(async () => {
			await readCacheEntry([lenBuf, Buffer.from('{"a":1')]);
		}, "Truncated cache entry");
	});

	it("corrupt metadata", async () => {
		const lenBuf = Buffer.alloc(4);
		lenBuf.writeUInt32BE(10, 0);
		await Exception.assertThrowsWithMatch(async () => {
			await readCacheEntry([lenBuf, Buffer.from("not-json!!")]);
		}, "Corrupt metadata");
	});

	it("round-trip integration", async () => {
		const originalMetadata = { key: "value", nested: { array: [1, 2, 3] } };
		const bodyStream = Readable.from([Buffer.from("hello "), Buffer.from("world")]);
		const entryStream = buildCacheEntryStream(originalMetadata, bodyStream);
		const fullBuffer = await buffer(entryStream);

		const { metadata, body } = await readCacheEntry([fullBuffer]);

		Exception.assertEqual(metadata, originalMetadata);
		Exception.assertEqual(body.toString(), "hello world");
	});

	it("one byte at a time", async () => {
		const stream = buildCacheEntryStream({ a: 1 }, Readable.from([Buffer.from("hello")]));
		const fullBuffer = await buffer(stream);

		const chunks = [];
		for (let i = 0; i < fullBuffer.length; i++) {
			chunks.push(fullBuffer.subarray(i, i + 1));
		}
		const { metadata, body } = await readCacheEntry(chunks);

		Exception.assertEqual(metadata, { a: 1 });
		Exception.assertEqual(body.toString(), "hello");
	});

	it("single chunk with length meta and body", async () => {
		const stream = buildCacheEntryStream({ a: 1 }, Readable.from([Buffer.from("hello")]));
		const fullBuffer = await buffer(stream);

		const { metadata, body } = await readCacheEntry([fullBuffer]);

		Exception.assertEqual(metadata, { a: 1 });
		Exception.assertEqual(body.toString(), "hello");
	});
});
