import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { arrayFindCommonPrefix } from "#bzd/nodejs/utils/array.mjs";

const Exception = ExceptionFactory("test", "array");

describe("Array", () => {
	describe("arrayFindCommonPrefix", () => {
		it("Simple", () => {
			Exception.assertEqual(arrayFindCommonPrefix(), []);
			Exception.assertEqual(arrayFindCommonPrefix([1]), [1]);
			Exception.assertEqual(arrayFindCommonPrefix([1, 2]), [1, 2]);
			Exception.assertEqual(arrayFindCommonPrefix([1, 2], [1]), [1]);
			Exception.assertEqual(arrayFindCommonPrefix([1, 2, 3], [1, 2, 3]), [1, 2, 3]);
			Exception.assertEqual(arrayFindCommonPrefix([1, 2, 3], [1, 2, 3], [2]), []);
			Exception.assertEqual(arrayFindCommonPrefix([2], [1, 2, 3], [1, 2, 3]), []);
			Exception.assertEqual(arrayFindCommonPrefix([1, 2, 3], [2], [1, 2, 3]), []);
			Exception.assertEqual(arrayFindCommonPrefix([1, 2, 3], [1, 2, 3], [1]), [1]);
			Exception.assertEqual(arrayFindCommonPrefix([1], [1, 2, 3], [1, 2, 3]), [1]);
			Exception.assertEqual(arrayFindCommonPrefix([1, 2, 3], [1], [1, 2, 3]), [1]);
			Exception.assertEqual(arrayFindCommonPrefix([1, 2, 3], [1, 2, 3], []), []);
			Exception.assertEqual(arrayFindCommonPrefix([], [1, 2, 3], [1, 2, 3]), []);
			Exception.assertEqual(arrayFindCommonPrefix([1, 2, 3], [], [1, 2, 3]), []);
		});
	});
});
