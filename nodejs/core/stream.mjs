import { Readable } from "stream";

export async function toBuffer(data) {
	return new Promise((resolve, reject) => {
		let chunks = [];
		data
			.on("data", (d) => chunks.push(d))
			.on("error", reject)
			.on("end", () => {
				resolve(Buffer.concat(chunks));
			});
	});
}

export function fromBuffer(buffer) {
	return new Readable({
		read() {
			this.push(buffer);
			this.push(null);
		},
	});
}
