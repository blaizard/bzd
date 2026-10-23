import unittest

from dependencies.interface import Dependencies


class TestDependencies(unittest.TestCase):
	def testDiff(self) -> None:
		before = Dependencies()
		before.add("rules_python", "2.4.2", None)
		before.add("rules_rust", "0.61.0", None)
		before.add("rules_rust", "0.68.1", None)
		before.add("bzd_lib", "1.0", None)

		after = Dependencies()
		after.add("rules_python", "2.4.4", None)
		after.add("rules_rust", "0.74.0", None)
		after.add("bzd_lib", "1.0", None)

		self.assertEqual(
			before.diff(after),
			[
				"rules_python: 2.4.2 -> 2.4.4",
				"rules_rust: 0.61.0, 0.68.1 -> 0.74.0",
			],
		)

	def testDiffWithoutChange(self) -> None:
		before = Dependencies()
		before.add("rules_python", "2.4.2", None)

		after = Dependencies()
		after.add("rules_python", "2.4.2", None)

		self.assertEqual(before.diff(after), [])


if __name__ == "__main__":
	unittest.main()
