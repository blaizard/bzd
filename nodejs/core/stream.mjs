import { Readable } from "stream";

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
export async function toString(readStream) {
	return new Promise((resolve, reject) => {
		const chunks = [];
		readStream
			.on("data", (d) => chunks.push(d))
			.on("error", reject)
			.on("end", () => {
				resolve(chunks.join(""));
			});
	});
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
