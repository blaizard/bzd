import sys
import unittest
import typing
from pathlib import Path

from tools.bzd2.grammar import Parser
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
			'comment': 'This is a multi-line comment\n\nSingle line\n\nSimple variable',
			'type': 'int16',
			'name': 'varSimple1'
			}
		}, {
			'attrs': {
			'const': '',
			'type': 'String',
			'name': 'varSimple2'
			}
		}, {
			'attrs': {
			'comment': 'Template variable',
			'type': 'List',
			'name': 'varTemplate1'
			},
			'template': [{
			'attrs': {
			'type': 'int32'
			}
			}]
		}, {
			'attrs': {
			'const': '',
			'type': 'Variant',
			'name': 'varTemplate2'
			},
			'template': [{
			'attrs': {
			'type': 'Boolean'
			}
			}, {
			'attrs': {
			'type': 'String'
			}
			}]
		}, {
			'attrs': {
			'comment': 'Complex nested template variable',
			'type': 'Variant',
			'name': 'varNested'
			},
			'template': [{
			'attrs': {
			'type': 'List'
			},
			'template': [{
			'attrs': {
			'type': 'Int'
			}
			}]
			}, {
			'attrs': {
			'type': 'Optional'
			},
			'template': [{
			'attrs': {
			'type': 'List'
			},
			'template': [{
			'attrs': {
			'type': 'Float'
			}
			}]
			}]
			}, {
			'attrs': {
			'type': 'Boolean'
			}
			}]
		}, {
			'attrs': {
			'comment': 'Initialized variable',
			'type': 'int16',
			'name': 'varInit1',
			'value': '12'
			}
		}, {
			'attrs': {
			'const': '',
			'type': 'int16',
			'name': 'varInit2',
			'value': '-5'
			}
		}, {
			'attrs': {
			'const': '',
			'type': 'String',
			'name': 'varInit3',
			'value': '"Hello World! <here>"'
			}
		}, {
			'attrs': {
			'type': 'Float',
			'name': 'varInit4',
			'value': '5.12'
			}
		}, {
			'attrs': {
			'comment': 'Contracts',
			'const': '',
			'type': 'int32',
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
			'name': 'defaultConstant',
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
			'type': 'interface',
			'name': 'MyFy'
			},
			'nested': [{
			'attrs': {
			'type': 'MyType',
			'name': 'var'
			}
			}, {
			'attrs': {
			'const': '',
			'type': 'MyType',
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
			'name': 'varInitialized',
			'value': '42'
			}
			}]
		}]

		self.assertListEqual(expected, result)


if __name__ == '__main__':
	TestRun.filePath = Path(sys.argv.pop())
	unittest.main()
