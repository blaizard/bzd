"use strict";

import ExceptionFactory from "./exception.js";

const Exception = ExceptionFactory("crypto");

export async function base64Encode(data) {
    if (process.env.BZD_RULE === "nodejs") {
        return (await import(/* webpackMode: "eager" */"./impl/crypto/buffer.base64.js")).default(data);
    }
    else if (process.env.BZD_RULE === "nodejs_web") {
        return (await import(/* webpackMode: "eager" */"./impl/crypto/window.btoa.js")).default(data);
    }
    Exception.unreachable("Unsupported environment: '{}'", process.env.BZD_RULE);
}
