import { Readable, PassThrough } from "stream";
import { pipeline } from "stream/promises";

/// Convert a read stream into a buffer.
export async function toBuffer(readStream) {
	return new Promise((resolve, reject) => {
		const chunks = [];
		readStream
			.on("data", (d) => chunks.push(d))
			.on("error", reject)
			.on("end", () => {
				resolve(Buffer.concat(chunks));
			});
	});
}

/// Convert a read stream into a string.
export async function toString(readStream, ...transforms) {
	const chunks = [];
	await pipeline(readStream, ...transforms, async (source) => {
		for await (const chunk of source) {
			chunks.push(chunk);
		}
	});
	return Buffer.concat(chunks).toString();
}

/// Create a stream from a string, Buffer or Uint8Array
export function fromChunk(data) {
	return new Readable({
		read() {
			this.push(data);
			this.push(null);
		},
	});
}

export async function copy(writeStream, readStream) {
	return new Promise((resolve, reject) => {
		readStream.on("error", reject).on("end", resolve).on("finish", resolve).pipe(writeStream);
	});
}

export async function* streamChunks(stream) {
	const reader = stream.getReader();
	try {
		while (true) {
			const { done, value } = await reader.read();
			if (done) {
				return;
			}
			yield value;
		}
	} finally {
		reader.releaseLock();
	}
}

/// Duplicates a readable stream into two independent PassThrough streams containing the same data.
export function teeReadStream(readStream) {
	const stream1 = new PassThrough();
	const stream2 = new PassThrough();

	readStream.pipe(stream1);
	readStream.pipe(stream2);

	readStream.on("error", (error) => {
		stream1.destroy(error);
		stream2.destroy(error);
	});

	return [stream1, stream2];
}
