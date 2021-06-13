import typing

from bzd.parser.parser import Parser as ParserBase
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.fragments import Fragment, FragmentNestedStart, FragmentNestedStop, FragmentNewElement, FragmentParentElement
from bzd.parser.element import Element

# Match all remaining content
_regexprContent = r"(?P<content>(.+?(?={{|{%)|.+))"
# Match a name
_regexprName = r"(?P<name>([a-zA-Z_\-0-9\.]+))"


def makeRegexprName(name: str) -> str:
	return r"(?P<" + name + r">([a-zA-Z_\-0-9\.]+))"


def makeGrammarContent() -> Grammar:
	"""
	Generate a grammar for the raw content.
	"""

	class FragmentContent(FragmentNewElement):
		default = {"category": "content"}

	return [GrammarItem(_regexprContent, FragmentContent)]


def makeGrammarSubstitution() -> Grammar:
	"""
	Generate a grammar for substitution blocks.
	"""

	class PipeStart(FragmentNestedStart):
		nestedName = "pipe"

	return [
		GrammarItem(r"{{", {"category": "substitution"}, [
		GrammarItemSpaces,
		GrammarItem(makeRegexprName("name"), Fragment, [
		GrammarItemSpaces,
		GrammarItem(r"\|", PipeStart, [
		GrammarItemSpaces,
		GrammarItem(makeRegexprName("name"), Fragment,
		[GrammarItemSpaces,
		GrammarItem(r"\|", FragmentNewElement),
		GrammarItem(r"}}", FragmentNestedStop)])
		]),
		GrammarItem(r"}}", FragmentNewElement)
		]),
		GrammarItem(r"}}", FragmentNewElement)
		])
	]


def makeGrammarControlFor() -> Grammar:
	"""
	Generate the grammar for the for loop control block.
	It matches the following:
		for value1 [, value2] in iterable %}
	"""

	grammarFromIn = [
		GrammarItemSpaces,
		GrammarItem(r"in", Fragment, [
		GrammarItemSpaces,
		GrammarItem(makeRegexprName("iterable"), Fragment,
		[GrammarItemSpaces, GrammarItem(r"%}", FragmentNestedStart, "root")])
		])
	]

	return [
		GrammarItem(r"for", {"category": "for"}, [
		GrammarItemSpaces,
		GrammarItem(makeRegexprName("value1"), Fragment, [
		GrammarItemSpaces,
		GrammarItem(r",", Fragment,
		[GrammarItemSpaces, GrammarItem(makeRegexprName("value2"), Fragment, grammarFromIn)])
		] + grammarFromIn)
		])
	]


def makeGrammarControlEnd() -> Grammar:
	"""
	Generate the grammar for the end control block.
	"""
	return [GrammarItem(r"end\s*%}", FragmentNestedStop)]


def makeGrammarControl() -> Grammar:
	"""
	Generate the grammar for all control blocks.
	"""

	return [GrammarItem(r"{%", Fragment, [
		GrammarItemSpaces,
	] + makeGrammarControlFor() + makeGrammarControlEnd())]


class Parser(ParserBase):

	def __init__(self, content: str) -> None:
		super().__init__(content,
			grammar=makeGrammarSubstitution() + makeGrammarControl() + makeGrammarContent(),
			defaultGrammar=[])
