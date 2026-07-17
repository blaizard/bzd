import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Data from "#bzd/nodejs/db/data/data.js";
import Provider from "../provider.js";

const Exception = ExceptionFactory("test", "statistics");

function makeProvider(...namespace) {
	const data = new Data();
	const provider = new Provider(...namespace);
	provider.proxy.register(data, {}, "proxyNs");
	return { provider, data };
}

async function assertGetValue(data, key, expectedValue) {
	const result = await data.get({ uid: "statistics", key });
	Exception.assert(result.hasValue(), "Expected data for key", key);
	Exception.assertEqual(result.value(), expectedValue);
}

async function assertGetWithUnit(data, key, expectedValue, expectedUnit) {
	const result = await data.get({ uid: "statistics", key, count: 1, metadata: true });
	Exception.assert(result.hasValue(), "Expected data for key", key);
	const entries = result.value();
	Exception.assert(entries.length >= 1);
	Exception.assertEqual(entries[0][1], expectedValue);
	Exception.assertEqual(entries[0][3], expectedUnit);
}

describe("Provider", () => {
	describe("constructor / set", () => {
		it("uses the constructor namespace in the key path", async () => {
			const { provider, data } = makeProvider("ctorNs");
			provider.set("x", 1);
			await assertGetValue(data, ["proxyNs", "ctorNs", "x"], 1);
		});

		it("passes options through untouched", async () => {
			const { provider, data } = makeProvider("ctorNs");
			provider.set("x", 1, { unit: "count" });
			await assertGetWithUnit(data, ["proxyNs", "ctorNs", "x"], 1, "count");
		});

		it("does nothing when no data is registered", () => {
			const provider = new Provider("noData");
			provider.set("x", 1);
		});
	});

	describe("size", () => {
		it("inserts value with unit 'By'", async () => {
			const { provider, data } = makeProvider("ctorNs");
			provider.size("mem", 1024);
			await assertGetWithUnit(data, ["proxyNs", "ctorNs", "mem"], 1024, "By");
		});
	});

	describe("sum", () => {
		it("inserts the accumulated value at [namespace..., name]", async () => {
			const { provider, data } = makeProvider("ctorNs");
			provider.sum("total", 5);
			await assertGetValue(data, ["proxyNs", "ctorNs", "total"], 5);
		});

		it("accumulates across calls", async () => {
			const { provider, data } = makeProvider("ctorNs");
			provider.sum("t", 1);
			await assertGetValue(data, ["proxyNs", "ctorNs", "t"], 1);
			provider.sum("t", 2);
			await assertGetValue(data, ["proxyNs", "ctorNs", "t"], 3);
			provider.sum("t", 3);
			await assertGetValue(data, ["proxyNs", "ctorNs", "t"], 6);
		});
	});

	describe("rate", () => {
		it("emits a 'rate' insert with unit 't/s' when process is invoked", async () => {
			const { provider, data } = makeProvider("ctorNs");
			provider.rate("ops", 8);
			const [nestedProvider, processor] = Object.values(provider.proxy.processors)[0];
			processor.process(2, nestedProvider);
			await assertGetWithUnit(data, ["proxyNs", "ctorNs", "ops", "rate"], 4, "t/s");
		});
	});

	describe("makeNested", () => {
		it("inserts with combined parent+child namespace", async () => {
			const { provider, data } = makeProvider("ctorNs");
			const child = provider.makeNested("child");
			provider.set("p", 1);
			await assertGetValue(data, ["proxyNs", "ctorNs", "p"], 1);
			child.set("c", 2);
			await assertGetValue(data, ["proxyNs", "ctorNs", "child", "c"], 2);
		});

		it("throws when no namespace is given", async () => {
			const { provider } = makeProvider("ctorNs");
			await Exception.assertThrowsWithMatch(() => provider.makeNested(), "Nested statistics must have a namespace");
		});
	});

	describe("timeit", () => {
		let originalNow;

		before(() => {
			originalNow = performance.now;
		});

		afterEach(() => {
			performance.now = originalNow;
		});

		it("inserts a duration under [namespace..., name] with unit 's'", async () => {
			const { provider, data } = makeProvider("ctorNs");
			let callCount = 0;
			performance.now = () => {
				callCount++;
				return callCount === 1 ? 1000 : 1500;
			};
			await provider.timeit("job", async () => {});
			await assertGetWithUnit(data, ["proxyNs", "ctorNs", "job"], 0.5, "s");
		});
	});
});
