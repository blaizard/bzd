import sys
import unittest
import typing

from tools.bdl.object import Object
from tools.bdl.visitors.result import Result
from tools.bdl.generators.bdl.visitor import formatBdl


class TestRun(unittest.TestCase):

	def testFormatter(self) -> None:

		bdl = Object.fromContent(content="""/*
  This is a multi-line comment
 */

// Contracts
const int32 defaultConstant [min = -1 , max = 35 ];

int32<Int, List<T<A/*Variable A*/, B, C<45>>>> complex;

interface MyFy {
                 MyType var;

/*
          A nested comment
         */

        const MyType<T> varConst [ /*    Immer */
        always ];
 MyType varInitialized = 42;    


      method myMethod() -> Integer;
}
""")
		output = formatBdl(bdl)

		self.assertRegex(output, r"interface MyFy")
		self.assertRegex(output, r"method myMethod")


if __name__ == '__main__':
	unittest.main()
