import * as fs from "fs";
import * as assert from "assert";

import { pathFromRLocation } from "#bzd_rules_nodejs/nodejs/runfiles/runfiles.js";

function getDataRLocation() {
	const index = process.argv.indexOf("--data");
	return process.argv[index + 1];
}

describe("testRunfiles", () => {
	it("Simple", () => {
		const rlocation = getDataRLocation();
		const path = pathFromRLocation(rlocation);
		const payload = fs.readFileSync(path, "utf8");
		assert.strictEqual(payload.trim(), "hello world");
	});
});
