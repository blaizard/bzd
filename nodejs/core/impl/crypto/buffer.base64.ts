import type { Base64Encoder } from "#bzd/nodejs/core/impl/crypto/types.js";

const base64: Base64Encoder = (data) => {
    return Buffer.from(data).toString("base64");
};

export default base64;