import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import DatabaseInfluxDB from "#bzd/apps/artifacts/plugins/nodes/databases/influxdb.js";

const Exception = ExceptionFactory("test", "artifacts", "plugins", "influxdb");

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
});
