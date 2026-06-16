import { Transform, Readable } from "stream";

// Store/read an entry on/from disk with the following format:
// - 4-byte length metadata
// - json metadata
// - body

export function buildCacheEntryStream(metadata, readStream) {
	const metadataSerialized = JSON.stringify(metadata);
	const metadataBuffer = Buffer.from(metadataSerialized, "utf8");
	const lengthBuffer = Buffer.alloc(4);
	lengthBuffer.writeUInt32BE(metadataBuffer.length, 0);

	return Readable.from(
		(async function* () {
			yield lengthBuffer;
			yield metadataBuffer;
			for await (const chunk of readStream) {
				yield chunk;
			}
		})(),
	);
}

export class CacheEntryReader extends Transform {
	constructor(onMetadata) {
		super();
		this.phase = "length";
		this.buffer = Buffer.alloc(0);
		this.metaLen = 0;
		this.onMetadata = onMetadata;
	}

	_transform(chunk, _enc, cb) {
		// Pass-through
		if (this.phase == "body") {
			this.push(chunk);
			return cb();
		}

		this.buffer = Buffer.concat([this.buffer, chunk]);

		if (this.phase == "length" && this.buffer.length >= 4) {
			this.metaLen = this.buffer.readUInt32BE(0);
			this.buffer = this.buffer.slice(4);
			this.phase = "meta";
		}

		if (this.phase == "meta" && this.buffer.length >= this.metaLen) {
			const metaBuf = this.buffer.slice(0, this.metaLen);
			const rest = this.buffer.slice(this.metaLen);

			let meta;
			try {
				meta = JSON.parse(metaBuf.toString("utf8"));
			} catch (e) {
				return cb(new Error("Corrupt metadata: " + e.message));
			}

			this.onMetadata(meta);
			this.buffer = Buffer.alloc(0);
			this.phase = "body";

			if (rest.length > 0) {
				this.push(rest);
			}
		}
		cb();
	}

	_flush(cb) {
		if (this.phase != "body") {
			return cb(new Error("Truncated cache entry"));
		}
		cb();
	}
}
