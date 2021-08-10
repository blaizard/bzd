import sys
import unittest
import typing
from pathlib import Path

from bzd.parser.element import ElementBuilder


class TestRun(unittest.TestCase):

	def testEqual(self) -> None:

		element1 = ElementBuilder().addAttrs({"a": "1", "b": "2"})

		self.assertEqual(element1, element1)

		element2 = ElementBuilder().addAttrs({"b": "2", "a": "1"})

		self.assertEqual(element1, element2)

		element2.addAttr("c", "3")

		self.assertNotEqual(element1, element2)
		self.assertNotEqual(element1, ElementBuilder())
		self.assertNotEqual(ElementBuilder(), element1)


if __name__ == '__main__':
	unittest.main()
