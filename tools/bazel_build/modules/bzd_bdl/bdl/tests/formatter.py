import unittest

from bdl.generators.bdl.visitor import formatBdl
from bdl.object import Object


class TestRun(unittest.TestCase):
	def _stripString(self, content: str) -> list[str]:
		return [line.strip() for line in content.split("\n") if line.strip()]

	def testFormatter(self) -> None:
		bdl = Object.fromContent(
			content="""/*
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
composition 
{
	var1 = 12 + ab.int32;
	regexpr1    = /12/-/da\\.ds.*/;
	}
"""
		)
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
}

composition {
	var1 = 12 + ab.int32;
	regexpr1 = /12/-/da\\.ds.*/;
}"""),
		)


if __name__ == "__main__":
	unittest.main()
