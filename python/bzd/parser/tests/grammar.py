import sys
import unittest
import typing
from pathlib import Path

from bzd.parser.parser import Parser
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.fragments import Fragment, FragmentNestedStart, FragmentNestedStop, FragmentNewElement, FragmentComment
from bzd.parser.element import Element
from bzd.parser.visitor import VisitorSerialize

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

		visitor = VisitorSerialize()
		result = visitor.visit(data)
		expected = [{'@': {'comment': {'v': 'I am a comment', 'i': 0}, 'const': {'v': '', 'i': 18}, 'variable': {'v': '', 'i': 24}, 'kind': {'v': 'int', 'i': 24}, 'name': {'v': 'myVar', 'i': 28}, 'value': {'v': '42', 'i': 36}}}, {'@': {'function': {'v': '', 'i': 40}, 'return': {'v': 'void', 'i': 40}, 'name': {'v': 'hello', 'i': 45}}, 'nested': [{'@': {'comment': {'v': 'I am a nested comment', 'i': 63}, 'variable': {'v': '', 'i': 96}, 'kind': {'v': 'float', 'i': 96}, 'name': {'v': 'nestedVar', 'i': 102}, 'value': {'v': '2', 'i': 114}}}]}]

		self.assertListEqual(expected, result)


if __name__ == '__main__':
	TestRun.filePath = Path(sys.argv.pop())
	unittest.main()
