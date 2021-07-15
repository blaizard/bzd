import sys
import unittest
import typing

from tools.bdl.grammar import Parser


class TestRun(unittest.TestCase):

	def testParser(self) -> None:
		parser = Parser(content="""varInit4 = Float(5.12);
// Contracts
defaultConstant = const int32 [min(-1) max(35)];
defaultConstant2 = int32(42) [min(-1) read];
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
			}
			},
			'values': [{
			'@': {
			'value': {
			'v': '-1',
			'i': 72
			}
			}
			}]
			}, {
			'@': {
			'type': {
			'v': 'max',
			'i': 76
			}
			},
			'values': [{
			'@': {
			'value': {
			'v': '35',
			'i': 80
			}
			}
			}]
			}]
		}, {
			'@': {
			'category': {
			'v': 'expression',
			'i': 86
			},
			'name': {
			'v': 'defaultConstant2',
			'i': 86
			},
			'type': {
			'v': 'int32',
			'i': 105
			}
			},
			'argument': [{
			'@': {
			'value': {
			'v': '42',
			'i': 111
			}
			}
			}],
			'contract': [{
			'@': {
			'type': {
			'v': 'min',
			'i': 116
			}
			},
			'values': [{
			'@': {
			'value': {
			'v': '-1',
			'i': 120
			}
			}
			}]
			}, {
			'@': {
			'type': {
			'v': 'read',
			'i': 124
			}
			}
			}]
		}, {
			'@': {
			'category': {
			'v': 'nested',
			'i': 131
			},
			'type': {
			'v': 'interface',
			'i': 131
			},
			'name': {
			'v': 'MyFy',
			'i': 141
			}
			},
			'nested': [{
			'@': {
			'comment': {
			'v': 'Send a message',
			'i': 149
			},
			'category': {
			'v': 'method',
			'i': 168
			},
			'name': {
			'v': 'send',
			'i': 175
			},
			'type': {
			'v': 'Result',
			'i': 209
			}
			},
			'argument': [{
			'@': {
			'category': {
			'v': 'expression',
			'i': 180
			},
			'name': {
			'v': 'message',
			'i': 180
			},
			'type': {
			'v': 'Sequence',
			'i': 190
			}
			},
			'template': [{
			'@': {
			'type': {
			'v': 'char',
			'i': 199
			}
			}
			}]
			}],
			'template': [{
			'@': {
			'type': {
			'v': 'int',
			'i': 216
			}
			}
			}]
			}, {
			'@': {
			'category': {
			'v': 'expression',
			'i': 223
			},
			'name': {
			'v': 'var',
			'i': 223
			},
			'type': {
			'v': 'MyType',
			'i': 229
			}
			}
			}, {
			'@': {
			'category': {
			'v': 'expression',
			'i': 238
			},
			'name': {
			'v': 'varConst',
			'i': 238
			},
			'const': {
			'v': '',
			'i': 249
			},
			'type': {
			'v': 'MyType',
			'i': 255
			}
			},
			'contract': [{
			'@': {
			'type': {
			'v': 'always',
			'i': 263
			}
			}
			}]
			}, {
			'@': {
			'category': {
			'v': 'expression',
			'i': 273
			},
			'name': {
			'v': 'varInitialized',
			'i': 273
			},
			'type': {
			'v': 'MyType',
			'i': 290
			}
			},
			'argument': [{
			'@': {
			'value': {
			'v': '42',
			'i': 297
			}
			}
			}]
			}]
		}]

		print(result)

		self.assertListEqual(expected, result)


if __name__ == '__main__':
	unittest.main()
