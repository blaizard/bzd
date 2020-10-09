import sys
import unittest
import typing
from pathlib import Path

from tools.bzd2.parser import Parser


class TestRun(unittest.TestCase):

	filePath: typing.Optional[Path] = None

	def testParser(self) -> None:
		assert self.filePath is not None
		parser = Parser(self.filePath)
		parser.parse()


if __name__ == '__main__':
	TestRun.filePath = Path(sys.argv.pop())
	unittest.main()
