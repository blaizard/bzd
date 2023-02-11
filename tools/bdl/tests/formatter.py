import sys
import unittest
import typing

from tools.bdl.object import Object
from tools.bdl.generators.bdl.visitor import formatBdl


class TestRun(unittest.TestCase):

	def _stripString(self, content: str) -> typing.List[str]:
		return [line.strip() for line in content.split("\n") if line.strip()]

	def testFormatter(self) -> None:

		bdl = Object.fromContent(content="""/*
  This is a multi-line comment
 */

struct   ab {
// Contracts
defaultConstant = const int32 [min(-1)   max(35) ];

complex = int32<Int, List<T<A/*Variable A*/, B, C<UInt>>>>;
}

interface MyFy {
		     var     =MyType;

/*
	    A nested comment
	   */

	  varConst = const MyType<T> [ /*    Immer */
	  integer ];
varInitialized= MyType(   42 );    


	method myMethod() -> Integer;
}
""")
		output = formatBdl(bdl)

		self.assertEqual(
			self._stripString(output),
			self._stripString("""
// This is a multi-line comment
struct ab {
	// Contracts
	defaultConstant = const int32 [min(-1) max(35)];
	complex = int32<Int, List<T</*Variable A*/ A, B, C<UInt>>>>;

}

interface MyFy {
	var = MyType;
	// 	    A nested comment
	varConst = const MyType<T> [/*Immer*/integer];
	varInitialized = MyType(42);
	method myMethod() -> Integer;

}"""))


if __name__ == '__main__':
	unittest.main()
