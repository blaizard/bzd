import sys
import unittest
import typing

from tools.bdl.grammar import Parser


class TestRun(unittest.TestCase):

	def testParser(self) -> None:
		parser = Parser(content="""Float varInit4 = 5.12;
// Contracts
const int32 defaultConstant [min = -1, max = 35];
int32 defaultConstant2 = 42 [min = -1, read];
interface MyFy
{
	// Send a message
	method send(Sequence<char> message) -> Result<int>;
	MyType var;
	const MyType varConst [always];
	MyType varInitialized = 42;
}""")
		data = parser.parse()

		result = data.serialize()
		expected = [{
			'@': {
			'type': {
			'v': 'Float',
			'i': 0
			},
			'category': {
			'v': 'variable',
			'i': 6
			},
			'name': {
			'v': 'varInit4',
			'i': 6
			},
			'value': {
			'v': '5.12',
			'i': 17
			}
			}
		}, {
			'@': {
			'comment': {
			'v': 'Contracts',
			'i': 23
			},
			'const': {
			'v': '',
			'i': 36
			},
			'type': {
			'v': 'int32',
			'i': 42
			},
			'category': {
			'v': 'variable',
			'i': 48
			},
			'name': {
			'v': 'defaultConstant',
			'i': 48
			}
			},
			'contract': [{
			'@': {
			'type': {
			'v': 'min',
			'i': 65
			},
			'value': {
			'v': '-1',
			'i': 71
			}
			}
			}, {
			'@': {
			'type': {
			'v': 'max',
			'i': 75
			},
			'value': {
			'v': '35',
			'i': 81
			}
			}
			}]
		}, {
			'@': {
			'type': {
			'v': 'int32',
			'i': 86
			},
			'category': {
			'v': 'variable',
			'i': 92
			},
			'name': {
			'v': 'defaultConstant2',
			'i': 92
			},
			'value': {
			'v': '42',
			'i': 111
			}
			},
			'contract': [{
			'@': {
			'type': {
			'v': 'min',
			'i': 115
			},
			'value': {
			'v': '-1',
			'i': 121
			}
			}
			}, {
			'@': {
			'type': {
			'v': 'read',
			'i': 125
			}
			}
			}]
		}, {
			'@': {
			'category': {
			'v': 'nested',
			'i': 132
			},
			'type': {
			'v': 'interface',
			'i': 132
			},
			'name': {
			'v': 'MyFy',
			'i': 142
			}
			},
			'nested': [{
			'@': {
			'comment': {
			'v': 'Send a message',
			'i': 150
			},
			'category': {
			'v': 'method',
			'i': 169
			},
			'name': {
			'v': 'send',
			'i': 176
			},
			'type': {
			'v': 'Result',
			'i': 208
			}
			},
			'argument': [{
			'@': {
			'type': {
			'v': 'Sequence',
			'i': 181
			},
			'category': {
			'v': 'variable',
			'i': 196
			},
			'name': {
			'v': 'message',
			'i': 196
			}
			},
			'template': [{
			'@': {
			'type': {
			'v': 'char',
			'i': 190
			}
			}
			}]
			}],
			'template': [{
			'@': {
			'type': {
			'v': 'int',
			'i': 215
			}
			}
			}]
			}, {
			'@': {
			'type': {
			'v': 'MyType',
			'i': 222
			},
			'category': {
			'v': 'variable',
			'i': 229
			},
			'name': {
			'v': 'var',
			'i': 229
			}
			}
			}, {
			'@': {
			'const': {
			'v': '',
			'i': 235
			},
			'type': {
			'v': 'MyType',
			'i': 241
			},
			'category': {
			'v': 'variable',
			'i': 248
			},
			'name': {
			'v': 'varConst',
			'i': 248
			}
			},
			'contract': [{
			'@': {
			'type': {
			'v': 'always',
			'i': 258
			}
			}
			}]
			}, {
			'@': {
			'type': {
			'v': 'MyType',
			'i': 268
			},
			'category': {
			'v': 'variable',
			'i': 275
			},
			'name': {
			'v': 'varInitialized',
			'i': 275
			},
			'value': {
			'v': '42',
			'i': 292
			}
			}
			}]
		}]

		self.assertListEqual(expected, result)


if __name__ == '__main__':
	unittest.main()
