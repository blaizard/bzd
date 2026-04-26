import ExceptionFactory from "../exception.mjs";
import ValidationSchema from "../validation_schema.mjs";

const Exception = ExceptionFactory("test", "validation-schema");

describe("Validation", () => {
	describe("simple", () => {
		it("wrong schema", () => {
			Exception.assertThrows(() => {
				new ValidationSchema("hello");
			});
		});
		it("string validation", () => {
			const validation = new ValidationSchema({
				properties: {
					id: {
						type: "string",
					},
				},
			});
			validation.validate({
				id: "hello",
			});
			Exception.assertThrows(() => {
				validation.validate({
					id: 12,
				});
			});
		});
		it("unknown validation type", () => {
			const validation = new ValidationSchema({
				properties: {
					id: {
						type: "sdkasd",
					},
				},
			});
			Exception.assertThrows(() => {
				validation.validate({
					id: "hello",
				});
			});
		});
		it("string enum validation", () => {
			const validation = new ValidationSchema({
				properties: {
					id: {
						type: "string",
						enum: ["hello", "world"],
					},
				},
			});
			validation.validate({
				id: "hello",
			});
			Exception.assertThrows(() => {
				validation.validate({
					id: "bonjour",
				});
			});
		});
		it("number validation", () => {
			const validation = new ValidationSchema({
				properties: {
					id: {
						type: "number",
					},
				},
			});
			validation.validate({
				id: 42,
			});
			Exception.assertThrows(() => {
				validation.validate({
					id: "hello",
				});
			});
		});
		it("integer validation", () => {
			const validation = new ValidationSchema({
				properties: {
					id: {
						type: "integer",
					},
				},
			});
			validation.validate({
				id: 42,
			});
			Exception.assertThrows(() => {
				validation.validate({
					id: 42.5,
				});
			});
		});
		it("boolean validation", () => {
			const validation = new ValidationSchema({
				properties: {
					id: {
						type: "boolean",
					},
				},
			});
			validation.validate({
				id: true,
			});
			Exception.assertThrows(() => {
				validation.validate({
					id: 42.5,
				});
			});
		});
		it("null validation", () => {
			const validation = new ValidationSchema({
				properties: {
					id: {
						type: "null",
					},
				},
			});
			validation.validate({
				id: null,
			});
			Exception.assertThrows(() => {
				validation.validate({
					id: {},
				});
			});
		});
		it("object validation", () => {
			const validation = new ValidationSchema({
				properties: {
					id: {
						type: "object",
						properties: {
							hello: {
								type: "string",
							},
						},
					},
				},
			});
			validation.validate({
				id: {
					hello: "world",
				},
			});
			Exception.assertThrows(() => {
				validation.validate({
					id: null,
				});
			});
			Exception.assertThrows(() => {
				validation.validate({
					id: {
						hello: false,
					},
				});
			});
		});
		it("array validation", () => {
			const validation = new ValidationSchema({
				properties: {
					id: {
						type: "array",
						items: {
							type: "string",
						},
					},
				},
			});
			validation.validate({
				id: ["hello"],
			});
			Exception.assertThrows(() => {
				validation.validate({
					id: ["hello", 12],
				});
			});
		});
		it("required validation", () => {
			const validation = new ValidationSchema({
				required: ["id", "hello"],
			});
			validation.validate({
				id: 12,
				hello: false,
			});
			Exception.assertThrows(() => {
				validation.validate({
					hello: true,
				});
			});
		});
		it("additionalProperties=true/default validation", () => {
			const validation = new ValidationSchema({
				properties: {
					id: {
						type: "boolean",
					},
				},
			});
			validation.validate({
				id: true,
				hello: "world",
			});
		});
		it("additionalProperties=false validation", () => {
			const validation = new ValidationSchema({
				properties: {
					id: {
						type: "boolean",
					},
				},
				additionalProperties: false,
			});
			validation.validate({
				id: true,
			});
			Exception.assertThrows(() => {
				validation.validate({
					id: true,
					hello: "world",
				});
			});
		});
	});
});
