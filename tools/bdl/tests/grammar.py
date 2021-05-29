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
			'comment': 'This is a multi-line comment,\nwith exactly 2 lines',
			'type': 'int16',
			'name': 'varSimple1',
			'category': 'variable'
			}
		}, {
			'attrs': {
			'const': '',
			'type': 'String',
			'name': 'varSimple2',
			'category': 'variable'
			}
		}, {
			'attrs': {
			'comment': 'Template variable',
			'type': 'List',
			'name': 'varTemplate1',
			'category': 'variable'
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
			'name': 'varTemplate2',
			'category': 'variable'
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
			'name': 'varNested',
			'category': 'variable'
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
			'value': '12',
			'category': 'variable'
			}
		}, {
			'attrs': {
			'const': '',
			'type': 'int16',
			'name': 'varInit2',
			'value': '-5',
			'category': 'variable'
			}
		}, {
			'attrs': {
			'const': '',
			'type': 'String',
			'name': 'varInit3',
			'value': '"Hello World! <here>"',
			'category': 'variable'
			}
		}, {
			'attrs': {
			'type': 'Float',
			'name': 'varInit4',
			'value': '5.12',
			'category': 'variable'
			}
		}, {
			'attrs': {
			'comment': 'Contracts',
			'const': '',
			'type': 'int32',
			'name': 'defaultConstant',
			'category': 'variable'
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
			'value': '42',
			'category': 'variable'
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
			'category': 'class',
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
			'template': [{
			'attrs': {
			'type': 'int'
			}
			}],
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
			}]
			}, {
			'attrs': {
			'type': 'MyType',
			'name': 'var',
			'category': 'variable'
			}
			}, {
			'attrs': {
			'const': '',
			'type': 'MyType',
			'name': 'varConst',
			'category': 'variable'
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
			'value': '42',
			'category': 'variable'
			}
			}]
		}, {
			'attrs': {
			'comment': 'Return hello world!',
			'category': 'method',
			'name': 'hello'
			},
			'argument': []
		}]

		print(result)
		self.assertListEqual(expected, result)


if __name__ == '__main__':
	TestRun.filePath = Path(sys.argv.pop())
	unittest.main()
