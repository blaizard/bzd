import sys
import unittest
import typing
from pathlib import Path

from tools.bdl.grammar import Parser
from bzd.parser.visitor import VisitorJson


class TestRun(unittest.TestCase):

	filePath: typing.Optional[Path] = None

	def testParser(self) -> None:
		assert self.filePath is not None
		parser = Parser(self.filePath)
		data = parser.parse()

		json = VisitorJson()
		result = json.visit(data)
		expected = [{
			'attrs': {
			'type': 'Float',
			'category': 'variable',
			'name': 'varInit4',
			'value': '5.12'
			}
		}, {
			'attrs': {
			'comment': 'Contracts',
			'const': '',
			'type': 'int32',
			'category': 'variable',
			'name': 'defaultConstant'
			},
			'contract': [{
			'attrs': {
			'type': 'min',
			'value': '-1'
			}
			}, {
			'attrs': {
			'type': 'max',
			'value': '35'
			}
			}]
		}, {
			'attrs': {
			'type': 'int32',
			'category': 'variable',
			'name': 'defaultConstant2',
			'value': '42'
			},
			'contract': [{
			'attrs': {
			'type': 'min',
			'value': '-1'
			}
			}, {
			'attrs': {
			'type': 'read'
			}
			}]
		}, {
			'attrs': {
			'category': 'nested',
			'type': 'interface',
			'name': 'MyFy'
			},
			'nested': [{
			'attrs': {
			'comment': 'Send a message',
			'category': 'method',
			'name': 'send',
			'type': 'Result'
			},
			'argument': [{
			'attrs': {
			'type': 'Sequence',
			'category': 'variable',
			'name': 'message'
			},
			'template': [{
			'attrs': {
			'type': 'char'
			}
			}]
			}],
			'template': [{
			'attrs': {
			'type': 'int'
			}
			}]
			}, {
			'attrs': {
			'type': 'MyType',
			'category': 'variable',
			'name': 'var'
			}
			}, {
			'attrs': {
			'const': '',
			'type': 'MyType',
			'category': 'variable',
			'name': 'varConst'
			},
			'contract': [{
			'attrs': {
			'type': 'always'
			}
			}]
			}, {
			'attrs': {
			'type': 'MyType',
			'category': 'variable',
			'name': 'varInitialized',
			'value': '42'
			}
			}]
		}]

		self.assertListEqual(expected, result)


if __name__ == '__main__':
	TestRun.filePath = Path(sys.argv.pop())
	unittest.main()
