import { ExceptionFactory, ExceptionPrecondition } from "../exception.js";
import Validation from "../validation.js";

const Exception = ExceptionFactory("test", "validation");

describe("Validation", () => {
	describe("throw_precondition", () => {
		it("throws a base exception by default", () => {
			const validator = new Validation({
				test: "mandatory",
			});
			Exception.assertThrows(() => {
				validator.validate({});
			});
			try {
				validator.validate({});
			} catch (e) {
				Exception.assert(!(e instanceof ExceptionPrecondition), "The error must not be a precondition.");
			}
		});
		it("throws a precondition with the 'throw_precondition' output", () => {
			const validator = new Validation({
				test: "mandatory",
			});
			try {
				validator.validate({}, { output: "throw_precondition" });
			} catch (e) {
				Exception.assert(e instanceof ExceptionPrecondition, "The error must be a precondition: '{}'", e);
				Exception.assertEqual(e.message, "'test' does not validate: mandatory");
			}
		});
		it("does not throw when all values are valid", () => {
			const validator = new Validation({
				test: "mandatory",
			});
			validator.validate({ test: "ok" }, { output: "throw_precondition" });
		});
	});
	describe("min", () => {
		it("mandatory min", () => {
			const validator = new Validation({
				test: "mandatory min(5)",
			});
			validator.validate({
				test: "Hello",
			});
			Exception.assertThrows(() => {
				validator.validate({
					test: "Me",
				});
			});
			Exception.assertThrows(() => {
				validator.validate({
					hello: "Me",
				});
			});
		});
		it("integer min", () => {
			const validator = new Validation({
				test: "type(integer) min(5)",
			});
			validator.validate({
				test: 6,
			});
			Exception.assertThrows(() => {
				validator.validate({
					test: "Me",
				});
			});
		});
		it("float min", () => {
			const validator = new Validation({
				test: "type(float) min(-5.78)",
			});
			validator.validate({
				test: -5.77,
			});
			Exception.assertThrows(() => {
				validator.validate({
					test: -5.79,
				});
			});
		});
		it("wrong number of arguments", () => {
			Exception.assertThrows(() => {
				new Validation({
					test: "type",
				});
			});
			Exception.assertThrows(() => {
				new Validation({
					test: "type()",
				});
			});
			Exception.assertThrows(() => {
				new Validation({
					test: "type(float,integer)",
				});
			});
		});
		it("values", () => {
			const validator = new Validation({
				test: "values(hello,world)",
			});
			validator.validate({
				test: "world",
			});
			Exception.assertThrows(() => {
				validator.validate({
					test: "me",
				});
			});
		});
	});
});
