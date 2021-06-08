import sys
import unittest
import typing
from pathlib import Path

from bzd.parser.tests.support.parser import TestParser
from bzd.parser.visitor import VisitorSerialize


class TestRun(unittest.TestCase):

	filePath: typing.Optional[Path] = None

	def testFromPath(self) -> None:
		assert self.filePath is not None
		parser = TestParser.fromPath(self.filePath)
		data = parser.parse()

		visitor = VisitorSerialize()
		result = visitor.visit(data)
		expected = [{
			'@': {
			'comment': {
			'v': 'I am a comment',
			'i': 0
			},
			'const': {
			'v': '',
			'i': 18
			},
			'variable': {
			'v': '',
			'i': 24
			},
			'kind': {
			'v': 'int',
			'i': 24
			},
			'name': {
			'v': 'myVar',
			'i': 28
			},
			'value': {
			'v': '42',
			'i': 36
			}
			}
		}, {
			'@': {
			'function': {
			'v': '',
			'i': 40
			},
			'return': {
			'v': 'void',
			'i': 40
			},
			'name': {
			'v': 'hello',
			'i': 45
			}
			},
			'nested': [{
			'@': {
			'comment': {
			'v': 'I am a nested comment',
			'i': 63
			},
			'variable': {
			'v': '',
			'i': 96
			},
			'kind': {
			'v': 'float',
			'i': 96
			},
			'name': {
			'v': 'nestedVar',
			'i': 102
			},
			'value': {
			'v': '2',
			'i': 114
			}
			}
			}]
		}]

		self.assertListEqual(expected, result)

	def testFromContent(self) -> None:
		parser = TestParser("float myVar = 42;")
		data = parser.parse()

		visitor = VisitorSerialize()
		result = visitor.visit(data)
		expected = [{
			'@': {
			'variable': {
			'v': '',
			'i': 0
			},
			'kind': {
			'v': 'float',
			'i': 0
			},
			'name': {
			'v': 'myVar',
			'i': 6
			},
			'value': {
			'v': '42',
			'i': 14
			}
			}
		}]

		self.assertListEqual(expected, result)


if __name__ == '__main__':
	TestRun.filePath = Path(sys.argv.pop())
	unittest.main()
