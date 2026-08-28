import type { Base64Encoder } from "#bzd/nodejs/core/impl/crypto/types.js";

const base64: Base64Encoder = (data) => {
    return window.btoa(data);
};

export default base64;