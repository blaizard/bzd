import sys
import unittest
import typing
from pathlib import Path

from tools.bdl.grammar import Parser
from tools.bdl.visitors.result import Result
from tools.bdl.visitors.map import Map
from tools.bdl.lang.bdl.visitor import formatBdl


class TestRun(unittest.TestCase):

	filePath: typing.Optional[Path] = None

	def testFormatter(self) -> None:
		assert self.filePath is not None
		parser = Parser.fromPath(self.filePath)
		data = parser.parse()

		symbols = Map().visit(data)

		result = Result(symbols).visit(data)

		bdl = formatBdl(result)

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

		print(bdl)

		self.assertEqual(expected, bdl)


if __name__ == '__main__':
	TestRun.filePath = Path(sys.argv.pop())
	unittest.main()
