import sys
import unittest
import typing
from pathlib import Path

from bzd.parser.parser import Parser
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.fragments import Fragment, FragmentNestedStart, FragmentNestedStop, FragmentNewElement, FragmentComment
from bzd.parser.element import Element
from bzd.parser.visitor import VisitorJson

# Simple Grammar

_grammarComment = [GrammarItem(r"//(?P<comment>[^\n]*)", FragmentComment)]
_grammarVariableDeclaration = [
	GrammarItem(r"const", {"const": ""}),
	GrammarItem(r"(?P<kind>int|float)", {"variable": ""}, [
	GrammarItem(r"(?P<name>[a-zA-Z_]+)", Fragment, [
	GrammarItem(r";", FragmentNewElement),
	GrammarItem(r"=", Fragment,
	[GrammarItem(r"(?P<value>[0-9]+)", Fragment, [
	GrammarItem(r";", FragmentNewElement),
	])])
	]),
	]),
]
_grammarFunctionDeclaration = [
	GrammarItem(r"(?P<return>void|int|float)", {"function": ""}, [
	GrammarItem(r"(?P<name>[a-zA-Z_]+)\s*\(\s*\)", Fragment,
	[GrammarItem(r"{", FragmentNestedStart, _grammarVariableDeclaration + [GrammarItem(r"}", FragmentNestedStop, [])])])
	]),
]

_grammar: Grammar = _grammarVariableDeclaration + _grammarFunctionDeclaration


class TestRun(unittest.TestCase):

	filePath: typing.Optional[Path] = None

	def testParser(self) -> None:
		assert self.filePath is not None
		parser = Parser(self.filePath, _grammar, [GrammarItemSpaces] + _grammarComment)
		data = parser.parse()

		visitor = VisitorJson()
		result = visitor.visit(data)
		expected = [{
			'attrs': {
			'comment': 'I am a comment',
			'const': '',
			'variable': '',
			'kind': 'int',
			'name': 'myVar',
			'value': '42'
			}
		}, {
			'attrs': {
			'function': '',
			'return': 'void',
			'name': 'hello'
			},
			'nested': [{
			'attrs': {
			'comment': 'I am a nested comment',
			'variable': '',
			'kind': 'float',
			'name': 'nestedVar',
			'value': '2'
			}
			}]
		}]

		self.assertListEqual(expected, result)


if __name__ == '__main__':
	TestRun.filePath = Path(sys.argv.pop())
	unittest.main()
