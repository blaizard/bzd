import unittest
from unittest import mock

from rust.bdl.generator.impl.builtins import ArrayType, ResultType, SpanType, VectorType


class TestBuiltins(unittest.TestCase):
	def testResultEmpty(self) -> None:
		result = ResultType.toType(entity=mock.MagicMock(), nested=[], reference=False, values=None)
		self.assertEqual(result, ("Result", ["()", "bzd::base::error::Error"]))

	def testResultOneType(self) -> None:
		result = ResultType.toType(entity=mock.MagicMock(), nested=["i32"], reference=False, values=None)
		self.assertEqual(result, ("Result", ["i32", "bzd::base::error::Error"]))

	def testResultTwoTypes(self) -> None:
		result = ResultType.toType(entity=mock.MagicMock(), nested=["i32", "bool"], reference=False, values=None)
		self.assertEqual(result, ("Result", ["i32", "bool"]))

	def testSpan(self) -> None:
		result = SpanType.toType(entity=mock.MagicMock(), nested=["i32"], reference=False, values=None)
		self.assertEqual(result, ("&'static [i32]", []))

	def testArray(self) -> None:
		entity = mock.MagicMock()
		entity.contracts.get.return_value = mock.MagicMock(valueNumber=4)
		result = ArrayType.toType(entity=entity, nested=["i32"], reference=False, values=None)
		self.assertEqual(result, ("&[i32; 4]", []))

	def testArrayDefaultCapacity(self) -> None:
		entity = mock.MagicMock()
		entity.contracts.get.return_value = None
		result = ArrayType.toType(entity=entity, nested=["i32"], reference=False, values=None)
		self.assertEqual(result, ("&[i32; 1]", []))

	def testVector(self) -> None:
		result = VectorType.toType(entity=mock.MagicMock(), nested=["i32"], reference=False, values=None)
		self.assertEqual(result, ("&[i32]", []))


if __name__ == "__main__":
	unittest.main()
