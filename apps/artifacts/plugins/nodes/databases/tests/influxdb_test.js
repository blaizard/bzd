import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import DatabaseInfluxDB from "#bzd/apps/artifacts/plugins/nodes/databases/influxdb.js";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "influxdb");

const makeDatabase = ({ queries = [], values = [] } = {}) => {
	return new DatabaseInfluxDB(
		null,
		{
			host: "http://localhost",
			org: "org",
			bucket: "bucket",
			token: "token",
		},
		{
			HttpClientFactory: class HttpClientFactory {
				constructor() {}
				async get() {
					return {};
				}
				async post(url, body) {
					if (url === "/query") {
						queries.push(body.data);
						return { results: [{ series: [{ columns: ["time", "a"], values: values }] }] };
					}
					return {};
				}
			},
		},
	);
};

const fieldToKey = {
	"": [""],
	".": ["", ""],
	"a.b.c": ["a", "b", "c"],
	"a|x2eb.c": ["a.b", "c"],
	"|x2ea|x2eb|x2ec": [".a.b.c"],
	"a|x22b.c": ['a"b', "c"],
	"a|x27b.c": ["a'b", "c"],
	"a|x20|x09b.c": ["a \tb", "c"],
	"a|x3db.c": ["a=b", "c"],
	"|x5fa": ["_a"],
	"a_b._c": ["a_b", "_c"],
	"a|x5cb": ["a\\b"],
};

describe("Influxdb", () => {
	it("key <-> field", () => {
		for (const [field, key] of Object.entries(fieldToKey)) {
			Exception.assertEqual(
				DatabaseInfluxDB.fromKeyToField(key),
				field,
				"fromKeyToField({:?}) should be {}",
				key,
				field,
			);
			Exception.assertEqual(
				DatabaseInfluxDB.fromFieldToKey(field),
				key,
				"fromFieldToKey({}) should be {:?}",
				field,
				key,
			);
		}
	});

	describe("timestampToInflux", () => {
		it("converts milliseconds to nanoseconds", () => {
			const database = makeDatabase();
			Exception.assertEqual(database.timestampToInflux(1), "1000000");
			Exception.assertEqual(database.timestampToInflux(1000), "1000000001");
			Exception.assertEqual(database.timestampToInflux(1000.4), "1000000002");
		});

		it("increments the offset for identical timestamps", () => {
			const database = makeDatabase();
			Exception.assertEqual(database.timestampToInflux(1234), "1234000000");
			Exception.assertEqual(database.timestampToInflux(1234), "1234000001");
			Exception.assertEqual(database.timestampToInflux(1234), "1234000002");
		});

		it("wraps the offset around after a million records", () => {
			const database = makeDatabase();
			database.offset = 999999;
			Exception.assertEqual(database.timestampToInflux(1234), "1234999999");
			Exception.assertEqual(database.timestampToInflux(1234), "1234000000");
		});
	});

	describe("onExternal", () => {
		it("uses the exact nanosecond boundary for large timestamps", async () => {
			const queries = [];
			const database = makeDatabase({ queries: queries });
			await database.onExternal("hello", ["a"], 2, 1790742920029, null);
			Exception.assert(queries[0].includes("time > 1790742920029999999ns"));
		});

		it("bounded by after, uses the nanosecond boundary and returns newest first", async () => {
			const queries = [];
			const database = makeDatabase({
				queries: queries,
				values: [
					[1000, 1],
					[2000, 2],
				],
			});
			const output = await database.onExternal("hello", ["a"], 2, 1000, null);
			Exception.assert(queries[0].includes("time > 1000999999ns"));
			Exception.assert(queries[0].includes("ORDER BY time ASC"));
			Exception.assertEqual(output, [
				[2000, 2],
				[1000, 1],
			]);
		});

		it("bounded by before, keeps the millisecond bound and does not reverse", async () => {
			const queries = [];
			const database = makeDatabase({
				queries: queries,
				values: [
					[2000, 2],
					[1000, 1],
				],
			});
			const output = await database.onExternal("hello", ["a"], 2, null, 3000);
			Exception.assert(queries[0].includes("time < 3000ms"));
			Exception.assert(queries[0].includes("ORDER BY time DESC"));
			Exception.assertEqual(output, [
				[2000, 2],
				[1000, 1],
			]);
		});

		it("bounded by after and before, uses the nanosecond boundary and returns newest first", async () => {
			const queries = [];
			const database = makeDatabase({
				queries: queries,
				values: [
					[1000, 1],
					[2000, 2],
				],
			});
			const output = await database.onExternal("hello", ["a"], 2, 1000, 3000);
			Exception.assert(queries[0].includes("time > 1000999999ns AND time < 3000ms"));
			Exception.assertEqual(output, [
				[2000, 2],
				[1000, 1],
			]);
		});

		it("without bound, keeps the descending order", async () => {
			const queries = [];
			const database = makeDatabase({
				queries: queries,
				values: [
					[2000, 2],
					[1000, 1],
				],
			});
			const output = await database.onExternal("hello", ["a"], 2, null, null);
			Exception.assert(queries[0].includes("ORDER BY time DESC"));
			Exception.assertEqual(output, [
				[2000, 2],
				[1000, 1],
			]);
		});
	});
});
