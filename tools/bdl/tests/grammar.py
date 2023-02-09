import sys
import unittest
import typing

from tools.bdl.grammar import Parser


class TestRun(unittest.TestCase):

	def _clearSertializedForCompare(self, sequence: typing.List[typing.Any]) -> None:
		for element in sequence:
			for key in element.keys():
				if key == "@":
					element["@"] = {key: value["v"] for key, value in element["@"].items()}
				else:
					self._clearSertializedForCompare(element[key])

	def assertParserEqual(self, parser: Parser, match: typing.List[typing.Any]) -> None:
		"""
		Check that a parsed data contains only the specified properties.
		"""

		data = parser.parse()
		result = data.serialize()
		self._clearSertializedForCompare(result)
		self.assertListEqual(result, match)

	def testUse(self) -> None:
		parser = Parser(content="use \"Hello\"")
		self.assertParserEqual(parser, [{"@": {"category": "use", "value": "Hello"}}])

		parser = Parser(content="use \"a/normal/path.bdl\"")
		self.assertParserEqual(parser, [{"@": {"category": "use", "value": "a/normal/path.bdl"}}])

	def testNamespace(self) -> None:
		parser = Parser(content="namespace Simple;")
		self.assertParserEqual(parser, [{"@": {"category": "namespace"}, "name": [{"@": {"name": "Simple"}}]}])

		parser = Parser(content="namespace Composed.Name;")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "namespace"
			},
			"name": [{
			"@": {
			"name": "Composed"
			}
			}, {
			"@": {
			"name": "Name"
			}
			}]
		}])

	def testEnum(self) -> None:
		parser = Parser(content="enum MyName { VAL }")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "enum",
			"name": "MyName"
			},
			"values": [{
			"@": {
			"name": "VAL"
			}
			}]
		}])

		parser = Parser(content="enum MyName { VAL1, VAL2 }")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "enum",
			"name": "MyName"
			},
			"values": [{
			"@": {
			"name": "VAL1"
			}
			}, {
			"@": {
			"name": "VAL2"
			}
			}]
		}])

	def testUsing(self) -> None:
		parser = Parser(content="using MyType = Integer;")
		self.assertParserEqual(parser, [{"@": {"category": "using", "name": "MyType", "symbol": "Integer"}}])

		parser = Parser(content="using MyType = Complex<Type>;")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "using",
			"name": "MyType",
			"symbol": "Complex"
			},
			"template": [{
			"@": {
			"symbol": "Type"
			}
			}]
		}])

		parser = Parser(content="using MyType = const Integer;")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "using",
			"const": "",
			"name": "MyType",
			"symbol": "Integer"
			}
		}])

	def testMethod(self) -> None:
		parser = Parser(content="method simple();")
		self.assertParserEqual(parser, [{"@": {"category": "method", "name": "simple"}, "argument": []}])

		parser = Parser(content="method withArgs(a = int);")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "method",
			"name": "withArgs"
			},
			"argument": [{
			"@": {
			"category": "expression",
			"interface": None,
			"name": "a"
			},
			"fragments": [{
			"@": {
			"symbol": "int"
			}
			}]
			}]
		}])

		parser = Parser(content="method withMultiArgs(a = int, b = const float);")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "method",
			"name": "withMultiArgs"
			},
			"argument": [{
			"@": {
			"category": "expression",
			"interface": None,
			"name": "a",
			},
			"fragments": [{
			"@": {
			"symbol": "int"
			},
			}]
			}, {
			"@": {
			"category": "expression",
			"interface": None,
			"name": "b",
			},
			"fragments": [{
			"@": {
			"const": "",
			"symbol": "float"
			},
			}]
			}]
		}])

		parser = Parser(content="method withReturn() -> float;")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "method",
			"name": "withReturn",
			"symbol": "float"
			},
			"argument": []
		}])

		parser = Parser(content="method withArgs(a = Vector<Integer>(12));")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "method",
			"name": "withArgs"
			},
			"argument": [{
			"@": {
			"category": "expression",
			"interface": None,
			"name": "a",
			},
			"fragments": [{
			"@": {
			"symbol": "Vector"
			},
			"template": [{
			'@': {
			'symbol': 'Integer'
			}
			}],
			"argument": [{
			'@': {
			"category": "expression"
			},
			"fragments": [{
			"@": {
			"value": "12"
			},
			}]
			}]
			}],
			}]
		}])

	def testExpression(self) -> None:
		parser = Parser(content="var1 = Integer;")
		self.assertParserEqual(parser, [{
			'@': {
			'category': 'expression',
			'interface': None,
			'name': 'var1'
			},
			'fragments': [{
			'@': {
			'symbol': 'Integer'
			}
			}]
		}])

		parser = Parser(content="Integer;")
		self.assertParserEqual(parser, [{'@': {'category': 'expression'}, 'fragments': [{'@': {'symbol': 'Integer'}}]}])

		parser = Parser(content="var1 = Integer(12);")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"interface": None,
			"name": "var1",
			},
			"fragments": [{
			'@': {
			'symbol': 'Integer'
			},
			"argument": [{
			"@": {
			"category": "expression",
			},
			"fragments": [{
			"@": {
			"value": "12"
			}
			}]
			}]
			}]
		}])

		parser = Parser(content="var1 = Integer(12); var2 = Integer;")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"interface": None,
			"name": "var1",
			},
			"fragments": [{
			'@': {
			'symbol': 'Integer'
			},
			"argument": [{
			"@": {
			"category": "expression",
			},
			"fragments": [{
			"@": {
			"value": "12"
			}
			}]
			}]
			}]
		}, {
			'@': {
			'category': 'expression',
			'interface': None,
			'name': 'var2'
			},
			'fragments': [{
			'@': {
			'symbol': 'Integer'
			}
			}]
		}])

		parser = Parser(content="var1 = const Float(-2.5) [test(1)];")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"interface": None,
			"name": "var1",
			},
			"fragments": [{
			"@": {
			"const": "",
			"symbol": "Float"
			},
			"argument": [{
			"@": {
			"category": "expression",
			},
			"fragments": [{
			"@": {
			"value": "-2.5"
			}
			}]
			}]
			}],
			"contract": [{
			'@': {
			"type": "test"
			},
			"values": [{
			"@": {
			"value": "1"
			}
			}]
			}]
		}])

		parser = Parser(content="var1 = Integer(-2.5, 54); var2 = Integer;")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"interface": None,
			"name": "var1",
			},
			"fragments": [{
			"@": {
			"symbol": "Integer"
			},
			"argument": [{
			"@": {
			"category": "expression",
			},
			"fragments": [{
			"@": {
			"value": "-2.5"
			},
			}]
			}, {
			"@": {
			"category": "expression",
			},
			"fragments": [{
			"@": {
			"value": "54"
			},
			}]
			}]
			}],
		}, {
			'@': {
			'category': 'expression',
			'interface': None,
			'name': 'var2'
			},
			'fragments': [{
			'@': {
			'symbol': 'Integer'
			}
			}]
		}])

		parser = Parser(content="var1 = Integer(key = 12);")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"interface": None,
			"name": "var1",
			},
			"fragments": [{
			"@": {
			"symbol": "Integer"
			},
			"argument": [{
			"@": {
			"category": "expression",
			"name": "key",
			},
			"fragments": [{
			"@": {
			"value": "12"
			},
			}]
			}]
			}]
		}])

		parser = Parser(content="var1 = Integer(key = my.symbol);")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"interface": None,
			"name": "var1",
			},
			"fragments": [{
			"@": {
			"symbol": "Integer"
			},
			"argument": [{
			"@": {
			"category": "expression",
			"name": "key",
			},
			"fragments": [{
			"@": {
			"symbol": "my.symbol"
			}
			}]
			}]
			}]
		}])

		parser = Parser(content="var1 = Integer(key = Integer(12));")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"interface": None,
			"name": "var1"
			},
			"fragments": [{
			"@": {
			"symbol": "Integer"
			},
			"argument": [{
			"@": {
			"category": "expression",
			"name": "key"
			},
			"fragments": [{
			"@": {
			"symbol": "Integer"
			},
			"argument": [{
			'@': {
			"category": "expression"
			},
			"fragments": [{
			"@": {
			'value': '12'
			},
			}]
			}]
			}]
			}]
			}]
		}])

		parser = Parser(content="fct(key = 12);")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			},
			"fragments": [{
			"@": {
			"symbol": "fct",
			},
			"argument": [{
			"@": {
			"name": "key",
			"category": "expression",
			},
			"fragments": [{
			"@": {
			"value": "12"
			},
			}]
			}]
			}],
		}])

		parser = Parser(content="this.is.fqn(12);")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			},
			"fragments": [{
			"@": {
			"symbol": "this.is.fqn",
			},
			"argument": [{
			"@": {
			"category": "expression",
			},
			"fragments": [{
			"@": {
			"value": "12"
			},
			}]
			}]
			}],
		}])

		parser = Parser(content="var1: my.base.type = Integer();")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"interface": "my.base.type",
			"name": "var1",
			},
			"fragments": [{
			"@": {
			"symbol": "Integer",
			},
			"argument": []
			}],
		}])

		parser = Parser(content="var1 = Vector<Integer>();")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"interface": None,
			"name": "var1",
			},
			"fragments": [{
			"@": {
			"symbol": "Vector",
			},
			"template": [{
			'@': {
			'symbol': 'Integer'
			}
			}],
			"argument": []
			}],
		}])

		parser = Parser(content="var1 = Vector<Vector<Integer, String>>();")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"interface": None,
			"name": "var1",
			},
			"fragments": [{
			"@": {
			"symbol": "Vector",
			},
			"template": [{
			'@': {
			'symbol': 'Vector'
			},
			"template": [{
			'@': {
			'symbol': 'Integer'
			}
			}, {
			'@': {
			'symbol': 'String'
			}
			}]
			}],
			"argument": []
			}]
		}])


if __name__ == '__main__':
	unittest.main()
