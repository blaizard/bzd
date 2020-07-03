

export default function base64(data) {
	return Buffer.from(data).toString("base64");
}
