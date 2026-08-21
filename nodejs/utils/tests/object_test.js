import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { objectDifference, isObject, deepMerge, extendObject } from "#bzd/nodejs/utils/object.js";

const Exception = ExceptionFactory("test", "utils", "object");

describe("objectDifference", () => {
	it("Empty objects", () => {
		Exception.assertEqual(objectDifference({}, {}), {});
	});
	it("Identical objects", () => {
		Exception.assertEqual(objectDifference({ a: 1, b: "foo" }, { a: 1, b: "foo" }), {});
	});
	it("Key only in object1", () => {
		Exception.assertEqual(objectDifference({ a: 1, b: 2 }, { a: 1 }), { b: 2 });
	});
	it("Key only in object2", () => {
		Exception.assertEqual(objectDifference({ a: 1 }, { a: 1, b: 2 }), { b: 2 });
	});
	it("Different values", () => {
		Exception.assertEqual(objectDifference({ a: 1 }, { a: 2 }), { a: 2 });
	});
	it("Different types", () => {
		Exception.assertEqual(objectDifference({ a: 1 }, { a: "1" }), { a: "1" });
	});
	it("Nested objects are compared recursively", () => {
		Exception.assertEqual(objectDifference({ a: { b: 1, c: 1 } }, { a: { b: 1, c: 2 } }), { a: { c: 2 } });
	});
	it("Identical nested objects", () => {
		Exception.assertEqual(objectDifference({ a: { b: 1 } }, { a: { b: 1 } }), { a: {} });
	});
});

describe("isObject", () => {
	it("True for plain objects", () => {
		Exception.assertEqual(isObject({}), true);
		Exception.assertEqual(isObject({ a: 1 }), true);
	});
	it("False for arrays", () => {
		Exception.assertEqual(isObject([]), false);
	});
	it("False for null and undefined", () => {
		Exception.assertEqual(isObject(null), false);
		Exception.assertEqual(isObject(undefined), false);
	});
	it("False for primitives", () => {
		Exception.assertEqual(isObject("str"), false);
		Exception.assertEqual(isObject(42), false);
		Exception.assertEqual(isObject(true), false);
	});
});

describe("deepMerge", () => {
	it("Empty sources", () => {
		const target = { a: 1 };
		Exception.assertEqual(deepMerge(target), target);
	});
	it("Merges flat objects", () => {
		Exception.assertEqual(deepMerge({ a: 1 }, { b: 2 }), { a: 1, b: 2 });
	});
	it("Sources override the target", () => {
		Exception.assertEqual(deepMerge({ a: 1 }, { a: 2 }), { a: 2 });
	});
	it("Merges nested objects", () => {
		Exception.assertEqual(deepMerge({ a: { b: 1 } }, { a: { c: 2 } }), { a: { b: 1, c: 2 } });
	});
	it("Creates nested objects when missing", () => {
		Exception.assertEqual(deepMerge({}, { a: { b: 1 } }), { a: { b: 1 } });
	});
	it("Merges multiple sources", () => {
		Exception.assertEqual(deepMerge({}, { a: 1 }, { b: 2 }), { a: 1, b: 2 });
	});
});

describe("extendObject", () => {
	it("Accesses properties from the object and extension", () => {
		const extended = extendObject({ a: 1 }, { b: 2 });
		Exception.assertEqual(extended.a, 1);
		Exception.assertEqual(extended.b, 2);
		Exception.assertEqual(extended.missing, undefined);
	});
	it("Gives priority to the original object", () => {
		Exception.assertEqual(extendObject({ a: 1 }, { a: 2 }).a, 1);
	});
	it("Binds extension methods to the proxy", () => {
		const extended = extendObject(
			{ a: 1 },
			{
				b: 2,
				sum() {
					return this.a + this.b;
				},
			},
		);
		Exception.assertEqual(extended.sum(), 3);
	});
});
