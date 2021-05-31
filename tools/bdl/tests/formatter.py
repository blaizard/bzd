import sys
import unittest
import typing
from pathlib import Path

from tools.bdl.grammar import Parser
from tools.bdl.lang.bdl.visitor import BdlFormatter


class TestRun(unittest.TestCase):

	filePath: typing.Optional[Path] = None

	def testFormatter(self) -> None:
		assert self.filePath is not None
		parser = Parser(self.filePath)
		data = parser.parse()

		formatter = BdlFormatter()
		result = formatter.visit(data)

		expected = """/*
 * This is a multi-line comment
 * 
 * Contracts
 */
const int32 defaultConstant [min = -1, max = 35];
int32<Int, List<T</*Variable A*/ A, B, C<45>>>> complex;
interface MyFy
{
	method myMethod() -> void;
	MyType var;
	// A nested comment
	const MyType<T> varConst [/*Immer*/always];
	MyType varInitialized = 42;
}"""

		print(result)

		self.assertEqual(expected, result)


if __name__ == '__main__':
	TestRun.filePath = Path(sys.argv.pop())
	unittest.main()
