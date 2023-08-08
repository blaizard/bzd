import {Readable} from "stream";

export async function toBuffer(data) {
  return new Promise((resolve, reject) => {
	let chunks = [];
	data.on("data", (d) => chunks.push(d)).on("error", reject).on("end", () => { resolve(Buffer.concat(chunks)); });
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
  return new Promise(
	  (resolve,
	   reject) => { readStream.on("error", reject).on("end", resolve).on("finish", resolve).pipe(writeStream); });
}

export async function* streamChunks(stream) {
  const reader = stream.getReader();
  try {
	while (true) {
	  const {done, value} = await reader.read();
	  if (done) {
		return;
	  }
	  yield value;
	}
  } finally {
	reader.releaseLock();
  }
}
