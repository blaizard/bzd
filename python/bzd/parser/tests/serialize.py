import sys
import unittest
import typing
from pathlib import Path

from bzd.parser.tests.support.parser import TestParser
from bzd.parser.element import Sequence


class TestRun(unittest.TestCase):

	def testParser(self) -> None:
		parser = TestParser("""
		const int hello = 2;
		void fct()
		{
			int member;
		}
		""")
		data = parser.parse()

		# Serialize
		result = data.serialize()

		# Deserialize
		sequence = Sequence.fromSerialize(result)

		# Re-serialize
		result2 = sequence.serialize()

		self.assertListEqual(result, result2)


if __name__ == "__main__":
	unittest.main()
