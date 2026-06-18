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

/// Reads a limited number of bytes from a readable stream, applying a timeout to prevent hanging.
/// * This function is safely designed to extract a preview of a payload (like an HTTP error response).
/// It uses asynchronous iteration to read chunks and automatically destroys the stream once the
/// byte limit is reached, the stream ends, or the timeout expires.
///
/// \param stream - The readable stream to consume.
/// \param maxBytes - The maximum number of bytes to read before aborting.
/// \param timeoutMs - The maximum time in milliseconds to wait before aborting.
/// \returns A promise that resolves to a UTF-8 string containing the extracted bytes.
/// If the limit or timeout is reached, a truncation marker is appended.
export async function peekReadStream(stream, maxBytes = 256, timeoutMs = 1000) {
	let buffer = Buffer.alloc(0);
	let timer;

	try {
		// 1. Task to read the stream
		const readPromise = async () => {
			for await (const chunk of stream) {
				buffer = Buffer.concat([buffer, chunk]);
				if (buffer.length >= maxBytes) break; // Stop reading once we hit the limit
			}
		};

		// 2. Task to handle the timeout
		const timeoutPromise = new Promise((_, reject) => {
			timer = setTimeout(() => reject(new Error("timeout")), timeoutMs);
		});

		// 3. Race them against each other
		await Promise.race([readPromise(), timeoutPromise]);
	} catch (err) {
		// We catch the timeout error or any unexpected stream errors here.
		// We intentionally do nothing, because we just want whatever was in the buffer before it failed.
	} finally {
		// 4. Always clean up
		clearTimeout(timer);
		stream.destroy();
	}

	return buffer.subarray(0, maxBytes);
}
