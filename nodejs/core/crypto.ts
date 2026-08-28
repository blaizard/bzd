/// Encode a string to base64 using the platform backend.
export async function base64Encode(data: string): Promise<string> {
    return (await import("#bzd/nodejs/core/impl/crypto/backend.js")).default(data);
}