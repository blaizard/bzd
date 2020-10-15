import sys
import unittest
import typing
from pathlib import Path

from tools.bzd2.grammar import Parser
from bzd.parser.visitor import Visitor


class TestRun(unittest.TestCase):

	filePath: typing.Optional[Path] = None

	def testParser(self) -> None:
		assert self.filePath is not None
		parser = Parser(self.filePath)
		data = parser.parse()
		print(data)
		print("---------------------------")

		class Checker(Visitor):
			nestedKind = "children"

		checker = Checker()
		checker.visit(data.data)
		print(checker)

		#raise Exception()


if __name__ == '__main__':
	TestRun.filePath = Path(sys.argv.pop())
	unittest.main()
