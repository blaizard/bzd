import * as fs from "fs";
import * as assert from "assert";

import { decrypt } from "#bzd_lib/nodejs/secret.js";
import { pathFromRLocation } from "#bzd_rules_nodejs/nodejs/runfiles/runfiles.js";

function getKeyFile() {
	const index = process.argv.indexOf("--key-file");
	return pathFromRLocation(process.argv[index + 1]);
}

function getSecret() {
	const index = process.argv.indexOf("--secret");
	return pathFromRLocation(process.argv[index + 1]);
}

describe("testSecret", async () => {
	it("Decrypt", async () => {
		const payload = fs.readFileSync(getSecret(), "utf-8");
		const text = await decrypt(payload, getKeyFile());
		assert.strictEqual(text, "hello world");
	});
});
