"use strict";

import ExceptionFactory from "../exception.mjs";
import Validation from "../validation.mjs";

const Exception = ExceptionFactory("test", "validation");

describe("Validation", () => {
	describe("Simple", () => {
		it("mandatory min", () => {
			const validator = new Validation({
				test: "mandatory min(5)"
			});
			validator.validate({
				test: "Hello"
			});
			Exception.assertThrows(() => {
				validator.validate({
					test: "Me"
				});
			});
			Exception.assertThrows(() => {
				validator.validate({
					hello: "Me"
				});
			});
		});
		it("integer min", () => {
			const validator = new Validation({
				test: "type(integer) min(5)"
			});
			validator.validate({
				test: 6
			});
			Exception.assertThrows(() => {
				validator.validate({
					test: "Me"
				});
			});
		});
	});
});
