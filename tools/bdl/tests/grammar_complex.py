import sys
import unittest
import typing

from tools.bdl.grammar import Parser


class TestRun(unittest.TestCase):

	def testParser(self) -> None:
		parser = Parser(content="""varInit4 = Float(5.12);
// Contracts
defaultConstant = const int32 [min = -1, max = 35];
defaultConstant2 = int32(42) [min = -1, read];
interface MyFy
{
	// Send a message
	method send(message = Sequence<char>) -> Result<int>;
	var = MyType;
	varConst = const MyType [always];
	varInitialized = MyType(42);
}""")
		data = parser.parse()

		result = data.serialize()
		expected = [{
			'@': {
			'category': {
			'v': 'expression',
			'i': 0
			},
			'name': {
			'v': 'varInit4',
			'i': 0
			},
			'type': {
			'v': 'Float',
			'i': 11
			}
			},
			'argument': [{
			'@': {
			'value': {
			'v': '5.12',
			'i': 17
			}
			}
			}]
		}, {
			'@': {
			'comment': {
			'v': 'Contracts',
			'i': 24
			},
			'category': {
			'v': 'expression',
			'i': 37
			},
			'name': {
			'v': 'defaultConstant',
			'i': 37
			},
			'const': {
			'v': '',
			'i': 55
			},
			'type': {
			'v': 'int32',
			'i': 61
			}
			},
			'contract': [{
			'@': {
			'type': {
			'v': 'min',
			'i': 68
			},
			'value': {
			'v': '-1',
			'i': 74
			}
			}
			}, {
			'@': {
			'type': {
			'v': 'max',
			'i': 78
			},
			'value': {
			'v': '35',
			'i': 84
			}
			}
			}]
		}, {
			'@': {
			'category': {
			'v': 'expression',
			'i': 89
			},
			'name': {
			'v': 'defaultConstant2',
			'i': 89
			},
			'type': {
			'v': 'int32',
			'i': 108
			}
			},
			'argument': [{
			'@': {
			'value': {
			'v': '42',
			'i': 114
			}
			}
			}],
			'contract': [{
			'@': {
			'type': {
			'v': 'min',
			'i': 119
			},
			'value': {
			'v': '-1',
			'i': 125
			}
			}
			}, {
			'@': {
			'type': {
			'v': 'read',
			'i': 129
			}
			}
			}]
		}, {
			'@': {
			'category': {
			'v': 'nested',
			'i': 136
			},
			'type': {
			'v': 'interface',
			'i': 136
			},
			'name': {
			'v': 'MyFy',
			'i': 146
			}
			},
			'nested': [{
			'@': {
			'comment': {
			'v': 'Send a message',
			'i': 154
			},
			'category': {
			'v': 'method',
			'i': 173
			},
			'name': {
			'v': 'send',
			'i': 180
			},
			'type': {
			'v': 'Result',
			'i': 214
			}
			},
			'argument': [{
			'@': {
			'category': {
			'v': 'expression',
			'i': 185
			},
			'name': {
			'v': 'message',
			'i': 185
			},
			'type': {
			'v': 'Sequence',
			'i': 195
			}
			},
			'template': [{
			'@': {
			'type': {
			'v': 'char',
			'i': 204
			}
			}
			}]
			}],
			'template': [{
			'@': {
			'type': {
			'v': 'int',
			'i': 221
			}
			}
			}]
			}, {
			'@': {
			'category': {
			'v': 'expression',
			'i': 228
			},
			'name': {
			'v': 'var',
			'i': 228
			},
			'type': {
			'v': 'MyType',
			'i': 234
			}
			}
			}, {
			'@': {
			'category': {
			'v': 'expression',
			'i': 243
			},
			'name': {
			'v': 'varConst',
			'i': 243
			},
			'const': {
			'v': '',
			'i': 254
			},
			'type': {
			'v': 'MyType',
			'i': 260
			}
			},
			'contract': [{
			'@': {
			'type': {
			'v': 'always',
			'i': 268
			}
			}
			}]
			}, {
			'@': {
			'category': {
			'v': 'expression',
			'i': 278
			},
			'name': {
			'v': 'varInitialized',
			'i': 278
			},
			'type': {
			'v': 'MyType',
			'i': 295
			}
			},
			'argument': [{
			'@': {
			'value': {
			'v': '42',
			'i': 302
			}
			}
			}]
			}]
		}]

		print(result)

		self.assertListEqual(expected, result)


if __name__ == '__main__':
	unittest.main()
