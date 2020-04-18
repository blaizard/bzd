"use strict";

import ExceptionFactory from "../exception.js";
import Format from "../format.js";

const Exception = ExceptionFactory("test", "format");

describe("Format", () => {
	describe("Simple", () => {
		it("No substitution", () => {
			const text = Format("Hello world!");
			Exception.assertEqual(text, "Hello world!");
		});
		it("Single substitution", () => {
			let text = Format("Hello {}!", "you");
			Exception.assertEqual(text, "Hello you!");
		});
	});
});
