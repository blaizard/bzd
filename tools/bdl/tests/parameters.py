import unittest
import typing

from tools.bdl.entities.impl.fragment.parameters import Parameters
from tools.bdl.entities.builder import ElementBuilder

class TestRun(unittest.TestCase):

	def testSimple(self) -> None:

		element = ElementBuilder("builtin").addConfigValue(kind = "Integer", name = "a").addConfigValue(kind = "Integer", name = "b")
		params = Parameters(element=element, nestedKind="config")
		assert params.size() == 2
		copy = params.copy()
		assert copy.size() == 2

if __name__ == '__main__':
	unittest.main()
