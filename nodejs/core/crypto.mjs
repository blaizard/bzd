export async function base64Encode(data) {
	return (await import("#bzd/nodejs/core/impl/crypto/backend.mjs")).default(data);
}
