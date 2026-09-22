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
	describe("format braces in keys", () => {
		const phantomKey = '{"uid":"admin@admin.com","password":"1234"}';
		const expectedMessage = "'" + phantomKey + "' does not validate: Key '" + phantomKey + "' is missing";
		const makeValidator = () => {
			return new Validation({
				test: "mandatory",
			});
		};
		it("throws a precondition when a validation key contains format braces", () => {
			try {
				makeValidator().validate(
					{
						test: "ok",
						[phantomKey]: "",
					},
					{ output: "throw_precondition", all: true },
				);
				Exception.unreachable("Expected the validation to fail.");
			} catch (e) {
				Exception.assert(e instanceof ExceptionPrecondition, "The error must be a precondition: '{}'", e);
				Exception.assertEqual(e.message, expectedMessage);
			}
		});
		it("throws a base exception when a validation key contains format braces", () => {
			try {
				makeValidator().validate(
					{
						test: "ok",
						[phantomKey]: "",
					},
					{ all: true },
				);
				Exception.unreachable("Expected the validation to fail.");
			} catch (e) {
				Exception.assert(!(e instanceof ExceptionPrecondition), "The error must not be a precondition.");
				Exception.assert(
					!(e instanceof Error && e.message.startsWith("Expected a dictionary as argument")),
					"The error must not be a raw format error: '{}'",
					e,
				);
				Exception.assertEqual(e.message, expectedMessage);
			}
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
