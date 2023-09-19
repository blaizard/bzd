import unittest
import typing

from bdl.entities.impl.fragment.parameters import Parameters
from bdl.entities.builder import ElementBuilder
from bdl.entities.impl.expression import Expression


class TestRun(unittest.TestCase):

	def testSimple(self) -> None:
		element = (ElementBuilder("builtin").addConfigValue(symbol="Integer", name="a").addConfigValue(symbol="Integer",
		                                                                                               name="b"))
		params = Parameters(element=element, NestedElementType=Expression, nestedKind="config")
		assert params.size() == 2
		copy = params.copy()
		assert copy.size() == 2


if __name__ == "__main__":
	unittest.main()
