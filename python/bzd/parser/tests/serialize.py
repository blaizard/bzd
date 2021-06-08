import sys
import unittest
import typing
from pathlib import Path

from bzd.parser.tests.support.parser import TestParser
from bzd.parser.visitor import VisitorSerialize
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
		visitor = VisitorSerialize()
		result = visitor.visit(data)

		# Deserialize
		sequence = Sequence.fromSerialize(result)

		# Re-serialize
		result2 = visitor.visit(sequence)

		self.assertListEqual(result, result2)


if __name__ == '__main__':
	unittest.main()
