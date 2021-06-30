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
		self.assertParserEqual(parser, [{"@": {"category": "using", "name": "MyType", "type": "Integer"}}])

		parser = Parser(content="using MyType = Complex<Type>;")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "using",
			"name": "MyType",
			"type": "Complex"
			},
			"template": [{
			"@": {
			"type": "Type"
			}
			}]
		}])

		parser = Parser(content="using MyType = const Integer;")
		self.assertParserEqual(parser, [{"@": {"category": "using", "const": "", "name": "MyType", "type": "Integer"}}])

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
			"name": "a",
			"type": "int"
			}
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
			"name": "a",
			"type": "int"
			}
			}, {
			"@": {
			"category": "expression",
			"name": "b",
			"type": "float",
			"const": ""
			}
			}]
		}])

		parser = Parser(content="method withReturn() -> float;")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "method",
			"name": "withReturn",
			"type": "float"
			},
			"argument": []
		}])

	def testExpression(self) -> None:
		parser = Parser(content="var1 = Integer;")
		self.assertParserEqual(parser, [{"@": {"category": "expression", "name": "var1", "type": "Integer"}}])

		parser = Parser(content="Integer;")
		self.assertParserEqual(parser, [{"@": {"category": "expression", "type": "Integer"}}])

		parser = Parser(content="var1 = Integer(12);")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"name": "var1",
			"type": "Integer",
			},
			"argument": [{
			"@": {
			"value": "12"
			}
			}]
		}])

		parser = Parser(content="var1 = const Float(-2.5) [test = 1];")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"const": "",
			"name": "var1",
			"type": "Float",
			},
			"argument": [{
			"@": {
			"value": "-2.5"
			}
			}],
			"contract": [{
			"@": {
			"type": "test",
			"value": "1"
			}
			}]
		}])

		parser = Parser(content="var1 = Integer(-2.5, 54);")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"name": "var1",
			"type": "Integer",
			},
			"argument": [{
			"@": {
			"value": "-2.5"
			}
			}, {
			"@": {
			"value": "54"
			}
			}]
		}])

		parser = Parser(content="var1 = Integer(key = 12);")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"name": "var1",
			"type": "Integer",
			},
			"argument": [{
			"@": {
			"name": "key",
			"value": "12"
			}
			}]
		}])

		parser = Parser(content="var1 = Integer(key = my.symbol);")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"name": "var1",
			"type": "Integer",
			},
			"argument": [{
			"@": {
			"name": "key",
			"symbol": "my.symbol"
			}
			}]
		}])

		parser = Parser(content="fct(key = 12);")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"type": "fct",
			},
			"argument": [{
			"@": {
			"name": "key",
			"value": "12"
			}
			}]
		}])

		parser = Parser(content="this.is.fqn(12);")
		self.assertParserEqual(parser, [{
			"@": {
			"category": "expression",
			"type": "this.is.fqn",
			},
			"argument": [{
			"@": {
			"value": "12"
			}
			}]
		}])


if __name__ == '__main__':
	unittest.main()
