import unittest
import typing

from tools.bdl.entities.impl.fragment.parameters import Parameters
from tools.bdl.entities.builder import ElementBuilder
from tools.bdl.entities.impl.expression import Expression


class TestRun(unittest.TestCase):

	def testSimple(self) -> None:

		element = ElementBuilder("builtin").addConfigValue(symbol="Integer", name="a").addConfigValue(symbol="Integer",
		                                                                                            name="b")
		params = Parameters(element=element, NestedElementType=Expression, nestedKind="config")
		assert params.size() == 2
		copy = params.copy()
		assert copy.size() == 2


if __name__ == '__main__':
	unittest.main()
