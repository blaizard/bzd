import unittest
from bzd.utils.match import getMatchingWeight, sortMatchingWeight


class TestRun(unittest.TestCase):

	def testGetMatchingWeight(self) -> None:

		self.assertEqual(getMatchingWeight("hello", "hello"), 1)
		self.assertAlmostEqual(getMatchingWeight("hello", "ahello"), 0.9, places=1)
		self.assertAlmostEqual(getMatchingWeight("hello", "helloa"), 0.9, places=1)
		self.assertAlmostEqual(getMatchingWeight("hello", "helllo"), 0.8, places=1)
		self.assertAlmostEqual(getMatchingWeight("hello", "world"), 0, places=1)
		self.assertAlmostEqual(getMatchingWeight("hello", "hworld"), 0.3, places=1)
		self.assertAlmostEqual(getMatchingWeight("hello", "h"), 0.2, places=1)

	def testSortMatchingWeight(self) -> None:

		compareWithList = ["hello", "world", "hello2", "this.is.hella"]
		output = sortMatchingWeight("hella", compareWithList)
		self.assertEqual(output, ["this.is.hella", "hello", "hello2"])


if __name__ == "__main__":
	unittest.main()
