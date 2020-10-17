import sys
import unittest
import typing
from pathlib import Path

from tools.bzd2.grammar import Parser
from tools.bzd2.formatter import Formatter


class TestRun(unittest.TestCase):

	filePath: typing.Optional[Path] = None

	def testFormatter(self) -> None:
		assert self.filePath is not None
		parser = Parser(self.filePath)
		data = parser.parse()

		formatter = Formatter()
		result = formatter.visit(data)

		print(result)
		#raise Exception()


if __name__ == '__main__':
	TestRun.filePath = Path(sys.argv.pop())
	unittest.main()
