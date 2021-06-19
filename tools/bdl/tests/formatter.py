import sys
import unittest
import typing
from pathlib import Path

from tools.bdl.object import Object
from tools.bdl.visitors.result import Result
from tools.bdl.generators.bdl.visitor import formatBdl


class TestRun(unittest.TestCase):

	filePath: typing.Optional[Path] = None

	def testFormatter(self) -> None:
		assert self.filePath is not None

		bdl = Object.fromPath(path=self.filePath)
		output = formatBdl(bdl)

		print(output)

		self.assertRegex(output, r"interface MyFy")
		self.assertRegex(output, r"method myMethod")

if __name__ == '__main__':
	TestRun.filePath = Path(sys.argv.pop())
	unittest.main()
