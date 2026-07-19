import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import config from "#bzd/apps/artifacts/api/config.json" with { type: "json" };
import { ArtifactsBase } from "#bzd/apps/artifacts/api/nodejs/common.js";

const Exception = ExceptionFactory("test", "artifacts", "common");

describe("ArtifactsBase", () => {
	describe("pathToKey", () => {
		it("Simple path", () => {
			Exception.assertEqual(ArtifactsBase.pathToKey("foo/bar"), ["foo", "bar"]);
		});

		it("Parent references are resolved", () => {
			Exception.assertEqual(ArtifactsBase.pathToKey("foo/bar/../baz"), ["foo", "baz"]);
		});

		it("Empty path returns no parts", () => {
			Exception.assertEqual(ArtifactsBase.pathToKey(""), []);
		});
	});

	describe("remotes", () => {
		it("Sequences through sources", () => {
			const instance = new ArtifactsBase({ remotes: ["http://a", "http://b"] });
			const collected = [...instance.remotes()];
			Exception.assertEqual(collected, [
				["http://a", 0, 0],
				["http://b", 0, 0],
			]);
			Exception.assertEqual(instance.remote, null);
		});

		it("Retries saved remote 3 times then continues", () => {
			const instance = new ArtifactsBase({ remotes: ["http://a"] });
			instance.remote = "http://saved";
			const collected = [...instance.remotes()];
			Exception.assertEqual(collected, [
				["http://saved", 0, 2],
				["http://saved", 1, 2],
				["http://saved", 2, 2],
				["http://a", 0, 0],
			]);
			Exception.assertEqual(instance.remote, null);
		});
	});

	describe("toString", () => {
		it("Includes remotes and masks token", () => {
			const instance = new ArtifactsBase({
				uid: "u123",
				remotes: ["http://a", "http://b"],
				volume: "nodes",
				token: "secret",
			});
			Exception.assertEqual(
				instance.toString(),
				"<ArtifactsBase uid=u123 remotes=http://a,http://b volume=nodes token=xxx>",
			);
		});

		it("Omits volume and token when null", () => {
			const instance = new ArtifactsBase({
				remotes: ["http://a"],
				volume: null,
				token: null,
			});
			Exception.assertEqual(instance.toString(), "<ArtifactsBase remotes=http://a>");
		});
	});

	describe("Constructor defaults", () => {
		it("Defaults match config values", () => {
			const instance = new ArtifactsBase();
			Exception.assertEqual(instance.uid, null);
			Exception.assertEqual(instance.remoteSources, config.remotes);
			Exception.assertEqual(instance.volume, config.defaultNodeVolume);
			// Token should come from the env or config
			Exception.assert(instance.token === (process.env.BZD_NODE_TOKEN ?? config.token), "Token mismatch");
			// httpClient should be a function/class
			Exception.assert(typeof instance.httpClient === "function", "httpClient should be a function");
		});
	});
});
