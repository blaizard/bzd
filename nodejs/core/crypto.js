"use strict";

export async function base64Encode(data) {
	if (process.env.BZD_RULE === "nodejs_web") {
		return (await import(/* webpackMode: "eager" */"./impl/crypto/window.btoa.js")).default(data);
	}
	return (await import(/* webpackMode: "eager" */"./impl/crypto/buffer.base64.js")).default(data);
}
