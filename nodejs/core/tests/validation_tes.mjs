"use strict";

import ExceptionFactory from "../exception.mjs";
import Validation from "../validation.mjs";

const Exception = ExceptionFactory("test", "validation");

describe("Validation", () => {
	describe("Simple", () => {
		it("No substitution", () => {
			new Validation({
				test: "mandatory min(0)"
			});
			throw new Exception();
		});
	});
});
