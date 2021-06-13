from bzd.parser.parser import Parser as ParserBase
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.fragments import Fragment, FragmentNestedStart, FragmentNestedStop, FragmentNewElement, FragmentParentElement, FragmentComment
from bzd.parser.element import Element

# Match all remaining content
_regexprContent = r"(?P<content>(.+?(?={{|{%)|.+))"
# Match a name
_regexprName = r"(?P<name>([a-zA-Z_\-0-9\.]+))"


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
		GrammarItem(_regexprName, Fragment, [
		GrammarItemSpaces,
		GrammarItem(r"\|", PipeStart, [
		GrammarItemSpaces,
		GrammarItem(_regexprName, Fragment,
		[GrammarItemSpaces,
		GrammarItem(r"\|", FragmentNewElement),
		GrammarItem(r"}}", FragmentNestedStop)])
		]),
		GrammarItem(r"}}", FragmentNewElement)
		]),
		GrammarItem(r"}}", FragmentNewElement)
		])
	]


class Parser(ParserBase):

	def __init__(self, content: str) -> None:
		super().__init__(content, grammar=makeGrammarSubstitution() + makeGrammarContent(), defaultGrammar=[])
