import assert from "assert";
import { execFile } from "child_process";
import { promisify } from "util";
import { stat } from "fs/promises";

import { a, b, b_c, bin_hello_world } from "./config_nodejs.js";

const execFileAsync = promisify(execFile);

describe("config", function () {
	it("simple", () => {
		assert.strictEqual(a(), 42);
		assert.strictEqual(b_c(), "hello");
		assert.deepStrictEqual(b(), { c: "hello", d: false });
	});

	it("binary", async () => {
		const binaryPath = bin_hello_world();

		const stats = await stat(binaryPath);
		assert.ok(stats.isFile(), "binary is a file");

		const { stdout } = await execFileAsync(binaryPath);
		assert.strictEqual(stdout.trim(), "hello world");
	});
});
