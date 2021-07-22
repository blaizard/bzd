import sys
import unittest
import typing

from tools.bdl.object import Object
from tools.bdl.generators.bdl.visitor import formatBdl


class TestRun(unittest.TestCase):

	def testFormatter(self) -> None:

		bdl = Object.fromContent(content="""/*
  This is a multi-line comment
 */

struct   ab {
// Contracts
defaultConstant = const int32 [min(-1)   max(35) ];

complex = int32<Int, List<T<A/*Variable A*/, B, C<45>>>>;
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

		self.assertRegex(output, r"interface MyFy")
		self.assertRegex(output, r"method myMethod")


if __name__ == '__main__':
	unittest.main()
