import unittest

from apps.utils.json.json import loadAndRepair


class TestLoadAndRepair(unittest.TestCase):
	def testValidObject(self) -> None:
		content = '{"a": 1, "b": [true, false, null]}'
		self.assertEqual(loadAndRepair(content), {"a": 1, "b": [True, False, None]})

	def testValidArray(self) -> None:
		content = "[1, 2, 3]"
		self.assertEqual(loadAndRepair(content), [1, 2, 3])

	def testPreambleText(self) -> None:
		content = 'some preamble before\n{"key": "value"}'
		self.assertEqual(loadAndRepair(content), {"key": "value"})

	def testLongestValidCandidate(self) -> None:
		content = '{"a": 1} and {"b": {"c": [1, 2, 3]}}'
		self.assertEqual(loadAndRepair(content), {"b": {"c": [1, 2, 3]}})

	def testNestedObject(self) -> None:
		content = '{"outer": {"inner": [1, {"deep": "value"}]}}'
		self.assertEqual(loadAndRepair(content), {"outer": {"inner": [1, {"deep": "value"}]}})

	def testMultipleTopLevelObjects(self) -> None:
		content = '{"a": 1}{"b": 2}'
		self.assertEqual(loadAndRepair(content), {"a": 1})

	def testTrailingGarbage(self) -> None:
		content = '{"a": 1} trailing text here'
		self.assertEqual(loadAndRepair(content), {"a": 1})

	def testInvalidJsonRaises(self) -> None:
		content = "this is not json at all"
		with self.assertRaises(ValueError):
			loadAndRepair(content)

	def testUnclosedBracketsRaises(self) -> None:
		content = '{"a": [1, 2}'
		with self.assertRaises(ValueError):
			loadAndRepair(content)

	def testEscapedCharacters(self) -> None:
		content = '{"text": "hello \\"world\\" and \\"nested\\""}'
		self.assertEqual(loadAndRepair(content), {"text": 'hello "world" and "nested"'})

	def testEmptyStringRaises(self) -> None:
		content = ""
		with self.assertRaises(ValueError):
			loadAndRepair(content)

	def testWhitespaceOnlyRaises(self) -> None:
		content = "   \n\t  "
		with self.assertRaises(ValueError):
			loadAndRepair(content)


if __name__ == "__main__":
	unittest.main()
