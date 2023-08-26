export async function base64Encode(data) {
	return (await import("./impl/crypto/backend.mjs")).default(data);
}
