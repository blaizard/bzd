import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import Provider from "../provider.js";

const Exception = ExceptionFactory("test", "statistics");

describe("Provider", () => {
	describe("sum", () => {
		it("accumulates", () => {
			const provider = new Provider("test");
			provider.sum("x", 1);
		});
	});
});
