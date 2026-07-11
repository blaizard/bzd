import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Generator from "#bzd/nodejs/db/data/generator.js";
import Data from "#bzd/nodejs/db/data/data.js";

const Exception = ExceptionFactory("test", "db", "generator");

describe("Generator", () => {
	describe("getColumns", () => {
		it("returns sorted columns from inserted data", async () => {
			const data = new Data();
			data.insert(
				"uid1",
				[
					[["sensor", "temp"], 42],
					[["sensor", "humid"], 55],
				],
				1000,
			);
			const gen = new Generator(data, "uid1", ["sensor"], 1, 0, null);

			const columns = await gen.getColumns();
			Exception.assertEqual(columns, ["humid", "temp"]);
		});

		it("returns empty array when no data", async () => {
			const data = new Data();
			const gen = new Generator(data, "uid1", ["sensor"], 1, 0, null);

			const columns = await gen.getColumns();
			Exception.assertEqual(columns, []);
		});
	});

	describe("byTimestamp", () => {
		it("yields single key values in ascending timestamp order", async () => {
			const data = new Data();
			data.insert("uid1", [[["sensor", "a"], "v1"]], 1000);
			data.insert("uid1", [[["sensor", "a"], "v2"]], 1010);
			data.insert("uid1", [[["sensor", "a"], "v3"]], 1020);
			const gen = new Generator(data, "uid1", ["sensor"], 1, 0, null);

			const results = [];
			for await (const entry of gen.byTimestamp()) {
				results.push(entry);
			}
			Exception.assertEqual(results, [
				[1000, { a: "v1" }],
				[1010, { a: "v2" }],
				[1020, { a: "v3" }],
			]);
		});

		it("interleaves multiple keys by oldest timestamp", async () => {
			const data = new Data();
			data.insert("uid1", [[["sensor", "a"], "a_80"]], 80);
			data.insert("uid1", [[["sensor", "a"], "a_100"]], 100);
			data.insert("uid1", [[["sensor", "b"], "b_50"]], 50);
			data.insert("uid1", [[["sensor", "b"], "b_150"]], 150);
			const gen = new Generator(data, "uid1", ["sensor"], 1, 0, null);

			const results = [];
			for await (const entry of gen.byTimestamp()) {
				results.push(entry);
			}
			Exception.assertEqual(results, [
				[50, { b: "b_50" }],
				[80, { a: "a_80" }],
				[100, { a: "a_100" }],
				[150, { b: "b_150" }],
			]);
		});

		it("groups same-timestamp entries into one yield", async () => {
			const data = new Data();
			data.insert(
				"uid1",
				[
					[["sensor", "a"], "a_50"],
					[["sensor", "b"], "b_50"],
				],
				50,
			);
			data.insert(
				"uid1",
				[
					[["sensor", "a"], "a_100"],
					[["sensor", "b"], "b_100"],
				],
				100,
			);
			const gen = new Generator(data, "uid1", ["sensor"], 1, 0, null);

			const results = [];
			for await (const entry of gen.byTimestamp()) {
				results.push(entry);
			}
			Exception.assertEqual(results, [
				[50, { a: "a_50", b: "b_50" }],
				[100, { a: "a_100", b: "b_100" }],
			]);
		});

		it("respects before boundary", async () => {
			const data = new Data();
			data.insert("uid1", [[["sensor", "a"], "v0"]], 0);
			data.insert("uid1", [[["sensor", "a"], "v50"]], 50);
			data.insert("uid1", [[["sensor", "a"], "v100"]], 100);
			const gen = new Generator(data, "uid1", ["sensor"], 1, -1, 75);

			const results = [];
			for await (const entry of gen.byTimestamp()) {
				results.push(entry);
			}
			Exception.assertEqual(results, [
				[0, { a: "v0" }],
				[50, { a: "v50" }],
			]);
		});

		it("empty result when no data", async () => {
			const data = new Data();
			const gen = new Generator(data, "uid1", ["sensor"], 1, 0, null);

			const results = [];
			for await (const entry of gen.byTimestamp()) {
				results.push(entry);
			}
			Exception.assertEqual(results, []);
		});

		it("handles children == 0 case", async () => {
			const data = new Data();
			data.insert("uid1", [[["sensor"], "v1"]], 100);
			data.insert("uid1", [[["sensor"], "v2"]], 200);
			const gen = new Generator(data, "uid1", ["sensor"], 0, 0, null);

			const results = [];
			for await (const entry of gen.byTimestamp()) {
				results.push(entry);
			}
			Exception.assertEqual(results, [
				[100, { "": "v1" }],
				[200, { "": "v2" }],
			]);
		});
	});
});
