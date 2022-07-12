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
			'i': 0,
			'e': 10
			},
			'name': {
			'v': 'varInit4',
			'i': 0,
			'e': 10
			},
			'symbol': {
			'v': None,
			'i': 0,
			'e': 10
			},
			'type': {
			'v': 'Float',
			'i': 11,
			'e': 16
			}
			},
			'argument': [{
			'@': {
			'value': {
			'v': '5.12',
			'i': 17,
			'e': 21
			}
			}
			}]
		}, {
			'@': {
			'comment': {
			'v': 'Contracts',
			'i': -1,
			'e': 0
			},
			'category': {
			'v': 'expression',
			'i': 37,
			'e': 54
			},
			'symbol': {
			'v': None,
			'i': 37,
			'e': 54
			},
			'name': {
			'v': 'defaultConstant',
			'i': 37,
			'e': 54
			},
			'const': {
			'v': '',
			'i': 55,
			'e': 60
			},
			'type': {
			'v': 'int32',
			'i': 61,
			'e': 66
			}
			},
			'contract': [{
			'@': {
			'type': {
			'v': 'min',
			'i': 68,
			'e': 71
			}
			},
			'values': [{
			'@': {
			'value': {
			'v': '-1',
			'i': 72,
			'e': 74
			}
			}
			}]
			}, {
			'@': {
			'type': {
			'v': 'max',
			'i': 76,
			'e': 79
			}
			},
			'values': [{
			'@': {
			'value': {
			'v': '35',
			'i': 80,
			'e': 82
			}
			}
			}]
			}]
		}, {
			'@': {
			'category': {
			'v': 'expression',
			'i': 86,
			'e': 104
			},
			'symbol': {
			'v': None,
			'i': 86,
			'e': 104
			},
			'name': {
			'v': 'defaultConstant2',
			'i': 86,
			'e': 104
			},
			'type': {
			'v': 'int32',
			'i': 105,
			'e': 110
			}
			},
			'argument': [{
			'@': {
			'value': {
			'v': '42',
			'i': 111,
			'e': 113
			}
			}
			}],
			'contract': [{
			'@': {
			'type': {
			'v': 'min',
			'i': 116,
			'e': 119
			}
			},
			'values': [{
			'@': {
			'value': {
			'v': '-1',
			'i': 120,
			'e': 122
			}
			}
			}]
			}, {
			'@': {
			'type': {
			'v': 'read',
			'i': 124,
			'e': 128
			}
			}
			}]
		}, {
			'@': {
			'category': {
			'v': 'nested',
			'i': 131,
			'e': 140
			},
			'type': {
			'v': 'interface',
			'i': 131,
			'e': 140
			},
			'name': {
			'v': 'MyFy',
			'i': 141,
			'e': 145
			}
			},
			'interface': [{
			'@': {
			'comment': {
			'v': 'Send a message',
			'i': -1,
			'e': 0
			},
			'category': {
			'v': 'method',
			'i': 168,
			'e': 174
			},
			'name': {
			'v': 'send',
			'i': 175,
			'e': 179
			},
			'type': {
			'v': 'Result',
			'i': 209,
			'e': 215
			}
			},
			'argument': [{
			'@': {
			'category': {
			'v': 'expression',
			'i': 180,
			'e': 189
			},
			'name': {
			'v': 'message',
			'i': 180,
			'e': 189
			},
			'symbol': {
			'v': None,
			'i': 180,
			'e': 189
			},
			'type': {
			'v': 'Sequence',
			'i': 190,
			'e': 198
			}
			},
			'template': [{
			'@': {
			'type': {
			'v': 'char',
			'i': 199,
			'e': 203
			}
			}
			}]
			}],
			'template': [{
			'@': {
			'type': {
			'v': 'int',
			'i': 216,
			'e': 219
			}
			}
			}]
			}, {
			'@': {
			'category': {
			'v': 'expression',
			'i': 223,
			'e': 228
			},
			'name': {
			'v': 'var',
			'i': 223,
			'e': 228
			},
			'symbol': {
			'v': None,
			'i': 223,
			'e': 228
			},
			'type': {
			'v': 'MyType',
			'i': 229,
			'e': 235
			}
			}
			}, {
			'@': {
			'category': {
			'v': 'expression',
			'i': 238,
			'e': 248
			},
			'name': {
			'v': 'varConst',
			'i': 238,
			'e': 248
			},
			'symbol': {
			'v': None,
			'i': 238,
			'e': 248
			},
			'const': {
			'v': '',
			'i': 249,
			'e': 254
			},
			'type': {
			'v': 'MyType',
			'i': 255,
			'e': 261
			}
			},
			'contract': [{
			'@': {
			'type': {
			'v': 'always',
			'i': 263,
			'e': 269
			}
			}
			}]
			}, {
			'@': {
			'category': {
			'v': 'expression',
			'i': 273,
			'e': 289
			},
			'name': {
			'v': 'varInitialized',
			'i': 273,
			'e': 289
			},
			'symbol': {
			'v': None,
			'i': 273,
			'e': 289
			},
			'type': {
			'v': 'MyType',
			'i': 290,
			'e': 296
			}
			},
			'argument': [{
			'@': {
			'value': {
			'v': '42',
			'i': 297,
			'e': 299
			}
			}
			}]
			}]
		}]

		print(result)

		self.assertListEqual(expected, result)


if __name__ == '__main__':
	unittest.main()
