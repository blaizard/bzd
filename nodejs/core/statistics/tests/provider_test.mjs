import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Provider from "../provider.mjs";

const Exception = ExceptionFactory("test", "statistics");

describe("Provider", () => {
	describe("sum", () => {
		it("accumulates correctly across multiple calls", () => {
			const provider = new Provider("test");
			provider.sum("x", 1);
			provider.sum("x", 2);
			provider.sum("x", 3);
			Exception.assertEqual(provider.data.x.sum, 6);
		});
	});

	describe("size", () => {
		it("initializes summary statistics on first call", () => {
			const provider = new Provider("test");
			provider.size("x", 100);
			Exception.assertEqual(provider.data.x.size, 100);
			Exception.assertEqual(provider.data.x.count, 1);
			Exception.assertEqual(provider.data.x.max, 100);
			Exception.assertEqual(provider.data.x.min, 0);
			Exception.assertEqual(provider.data.x.avg, 100);
		});
	});
});
