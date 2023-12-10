import unittest
from bzd.utils.dict import updateDeep, UpdatePolicy


class TestRun(unittest.TestCase):

	def testUpdateDeepNoConflict(self) -> None:
		output = updateDeep({"a": 1}, {"b": 2})
		self.assertEqual(output, {"a": 1, "b": 2})
		output = updateDeep({"b": 2}, {"a": 1})
		self.assertEqual(output, {"a": 1, "b": 2})

		output = updateDeep({"a": 1}, {"b": 2}, policy=UpdatePolicy.raiseOnConflict)
		self.assertEqual(output, {"a": 1, "b": 2})
		output = updateDeep({"b": 2}, {"a": 1}, policy=UpdatePolicy.raiseOnConflict)
		self.assertEqual(output, {"a": 1, "b": 2})

		output = updateDeep({"a": {"b": 2}}, {"b": 2})
		self.assertEqual(output, {"a": {"b": 2}, "b": 2})
		output = updateDeep({"b": 2}, {"a": {"b": 2}})
		self.assertEqual(output, {"a": {"b": 2}, "b": 2})

		output = updateDeep({"a": {"b": 2}}, {"b": 2}, policy=UpdatePolicy.raiseOnConflict)
		self.assertEqual(output, {"a": {"b": 2}, "b": 2})
		output = updateDeep({"b": 2}, {"a": {"b": 2}}, policy=UpdatePolicy.raiseOnConflict)
		self.assertEqual(output, {"a": {"b": 2}, "b": 2})

		output = updateDeep({"a": {"b": 2}}, {"a": {"c": 3}})
		self.assertEqual(output, {"a": {"b": 2, "c": 3}})
		output = updateDeep({"a": {"c": 3}}, {"a": {"b": 2}})
		self.assertEqual(output, {"a": {"b": 2, "c": 3}})

		output = updateDeep({"a": {"b": 2}}, {"a": {"c": 3}}, policy=UpdatePolicy.raiseOnConflict)
		self.assertEqual(output, {"a": {"b": 2, "c": 3}})
		output = updateDeep({"a": {"c": 3}}, {"a": {"b": 2}}, policy=UpdatePolicy.raiseOnConflict)
		self.assertEqual(output, {"a": {"b": 2, "c": 3}})

	def testUpdateDeepEdgeCase(self) -> None:
		output = updateDeep({}, {})
		self.assertEqual(output, {})

	def testUpdateDeepOverride(self) -> None:
		output = updateDeep({"a": 1}, {"a": 2})
		self.assertEqual(output, {"a": 2})

		output = updateDeep({"a": {"b": 1}}, {"a": 2})
		self.assertEqual(output, {"a": 2})

		output = updateDeep({"a": 1}, {"a": {}})
		self.assertEqual(output, {"a": {}})

		output = updateDeep({"a": {}}, {"a": 1})
		self.assertEqual(output, {"a": 1})

		output = updateDeep({"a": {"b": 1}}, {"a": {"b": {"c": 1}}})
		self.assertEqual(output, {"a": {"b": {"c": 1}}})

		output = updateDeep({"a": {"b": {"c": 1}}}, {"a": {"b": 1}})
		self.assertEqual(output, {"a": {"b": 1}})

	def testUpdateDeepRaiseOnConflict(self) -> None:
		with self.assertRaisesRegex(KeyError, "'a'"):
			updateDeep({"a": 1}, {"a": 2}, policy=UpdatePolicy.raiseOnConflict)

		with self.assertRaisesRegex(KeyError, "'a'"):
			updateDeep({"a": {"b": 1}}, {"a": 2}, policy=UpdatePolicy.raiseOnConflict)

		with self.assertRaisesRegex(KeyError, "'a'"):
			updateDeep({"a": 1}, {"a": {}}, policy=UpdatePolicy.raiseOnConflict)

		with self.assertRaisesRegex(KeyError, "'a'"):
			updateDeep({"a": {}}, {"a": 1}, policy=UpdatePolicy.raiseOnConflict)

		with self.assertRaisesRegex(KeyError, "'a.b'"):
			updateDeep({"a": {"b": 1}}, {"a": {"b": {"c": 1}}}, policy=UpdatePolicy.raiseOnConflict)

		with self.assertRaisesRegex(KeyError, "'a.b'"):
			updateDeep({"a": {"b": {"c": 1}}}, {"a": {"b": 1}}, policy=UpdatePolicy.raiseOnConflict)

	def testUpdateDeepRaiseNonConflict(self) -> None:
		output = updateDeep({"a": 1}, {"a": 2}, policy=UpdatePolicy.raiseOnNonConflict)
		self.assertEqual(output, {"a": 2})

		with self.assertRaisesRegex(KeyError, "'b'"):
			updateDeep({"a": 1}, {"b": 2}, policy=UpdatePolicy.raiseOnNonConflict)

		output = updateDeep({"a": 1}, {"a": {"c": 2}}, policy=UpdatePolicy.raiseOnNonConflict)
		self.assertEqual(output, {"a": {"c": 2}})


if __name__ == "__main__":
	unittest.main()
