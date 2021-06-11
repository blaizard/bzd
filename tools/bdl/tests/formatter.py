import sys
import unittest
import typing
from pathlib import Path

from tools.bdl.object import Object
from tools.bdl.visitors.result import Result
from tools.bdl.lang.bdl.visitor import formatBdl


class TestRun(unittest.TestCase):

	filePath: typing.Optional[Path] = None

	def testFormatter(self) -> None:
		assert self.filePath is not None
		bdl = Object.fromPath(path=self.filePath)

		result = Result(bdl).process()

		output = formatBdl(result)

		expected = """/*
 * This is a multi-line comment
 * 
 * Contracts
 */
const int32 defaultConstant [min = -1, max = 35];
int32<Int, List<T</*Variable A*/ A, B, C<45>>>> complex;
interface MyFy
{
	method myMethod() -> Integer;
	MyType var;
	// A nested comment
	const MyType<T> varConst [/*Immer*/always];
	MyType varInitialized = 42;
}"""

		print(output)

		self.assertEqual(expected, output)


if __name__ == '__main__':
	TestRun.filePath = Path(sys.argv.pop())
	unittest.main()
