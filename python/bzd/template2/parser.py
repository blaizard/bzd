import typing

from bzd.parser.parser import Parser as ParserBase
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.fragments import Fragment, FragmentNestedStart, FragmentNestedStopNewElement, FragmentNewElement, FragmentParentElement
from bzd.parser.element import Element

# Match all remaining content
_regexprContent = r"(?P<content>([\s\S]+?(?={{|{%|{#)|.+))"
# Match a name
_regexprName = r"(?P<name>([a-zA-Z_\-0-9\.]+))"
# Match condition
_regexprCondition = r"(?P<condition>(.+?(?=-?%})))"
# Match comment
_regexprComment = r"(?P<comment>(.+?(?=-?#})))"


def makeRegexprName(name: str) -> str:
	return r"(?P<" + name + r">([a-zA-Z_\-0-9\.]+))"


def makeGrammarContent() -> Grammar:
	"""
	Generate a grammar for the raw content.
	"""

	class FragmentContent(FragmentNewElement):
		default = {"category": "content"}

	return [GrammarItem(_regexprContent, FragmentContent)]


def makeGrammarSubstitutionStart(grammar: Grammar) -> Grammar:
	return [GrammarItem(r"{{-", {"stripLeft": "1"}, grammar), GrammarItem(r"{{", Fragment, grammar)]


def makeGrammarControlStart(keyword: str, attribute: str, grammar: Grammar) -> Grammar:
	return [
		GrammarItem(r"{%\s*" + keyword, Fragment, grammar),
		GrammarItem(r"{%-\s*" + keyword, {attribute: "1"}, grammar)
	]


def makeGrammarCommentStart(grammar: Grammar) -> Grammar:
	return [
		GrammarItem(r"{#-", {"stripLeft": "1"}, grammar),
		GrammarItem(r"{#", Fragment, grammar),
	]


def makeGrammarSubstitutionStop(fragment: typing.Type[Fragment]) -> Grammar:
	return [
		GrammarItem(r"(?=}})", Fragment, [GrammarItem(r"}}", fragment)]),
		GrammarItem(r"(?=-}})", {"stripRight": "1"}, [GrammarItem(r"-}}", fragment)]),
	]


def makeGrammarControlStop(fragment: typing.Type[Fragment],
	grammar: typing.Optional[typing.Union[Grammar, str]] = None) -> Grammar:
	return [
		GrammarItem(r"(?=%})", Fragment, [GrammarItem(r"%}", fragment, grammar)]),
		GrammarItem(r"(?=-%})", {"stripRight": "1"}, [GrammarItem(r"-%}", fragment, grammar)]),
	]


def makeGrammarCommentStop(fragment: typing.Type[Fragment]) -> Grammar:
	return [
		GrammarItem(r"(?=#})", Fragment, [GrammarItem(r"#}", fragment)]),
		GrammarItem(r"(?=-#})", {"stripRight": "1"}, [GrammarItem(r"-#}", fragment)]),
	]


def makeGrammarSubstitution() -> Grammar:
	"""
	Generate a grammar for substitution blocks.
	"""

	class PipeStart(FragmentNestedStart):
		nestedName = "pipe"

	return makeGrammarSubstitutionStart([
		GrammarItem(makeRegexprName("name"), {"category": "substitution"}, [
		GrammarItem(r"\|", PipeStart, [
		GrammarItem(makeRegexprName("name"), Fragment,
		[GrammarItem(r"\|", FragmentNewElement),
		GrammarItem(None, FragmentParentElement)])
		])
		] + makeGrammarSubstitutionStop(FragmentNewElement)),
	] + makeGrammarSubstitutionStop(FragmentNewElement))


def makeGrammarControlFor() -> Grammar:
	"""
	Generate the grammar for the for loop control block.
	It matches the following:
		for value1 [, value2] in iterable %}
	"""

	grammarFromIn = [
		GrammarItem(r"in", Fragment,
		[GrammarItem(makeRegexprName("iterable"), Fragment, makeGrammarControlStop(FragmentNestedStart, "root"))])
	]

	return makeGrammarControlStart(r"for", "stripLeft", [
		GrammarItem(None, {"category": "for"}, [
		GrammarItem(makeRegexprName("value1"), Fragment,
		[GrammarItem(r",", Fragment, [GrammarItem(makeRegexprName("value2"), Fragment, grammarFromIn)])] +
		grammarFromIn)
		])
	])


def makeGrammarControlIf() -> Grammar:
	"""
	Generate the grammar for the if block.
	It matches the following:
		if condition %}
	"""
	return makeGrammarControlStart(r"if", "stripLeft", [
		GrammarItem(None, {"category": "if"},
		[GrammarItem(_regexprCondition, Fragment, makeGrammarControlStop(FragmentNestedStart, "root"))])
	])


def makeGrammarControlElseIf() -> Grammar:
	"""
	Generate the grammar for the else / elif block.
	It matches the following:
		else %}
		elif condition %}
	"""

	class ElseFragment(FragmentNestedStart):
		default = {"category": "else"}

	return [
		GrammarItem(
		r"(?={%-?\s*(else|elif))", FragmentNestedStopNewElement,
		makeGrammarControlStart(r"elif", "stripLeft",
		[GrammarItem(_regexprCondition, {"category": "else"}, makeGrammarControlStop(FragmentNestedStart, "root"))]) +
		makeGrammarControlStart(r"else", "stripLeft", makeGrammarControlStop(ElseFragment, "root")))
	]


def makeGrammarControlEnd() -> Grammar:
	"""
	Generate the grammar for the end control block.
	"""

	class FragmentEndElement(FragmentNewElement):
		default = {"category": "end"}

	return [
		GrammarItem(
		r"(?={%-?\s*end)", FragmentParentElement,
		makeGrammarControlStart(r"end", "nestedStripRight",
		[GrammarItem(None, FragmentNewElement, makeGrammarControlStop(FragmentEndElement))]))
	]


def makeGrammarControl() -> Grammar:
	"""
	Generate the grammar for all control blocks.
	"""

	return [
		GrammarItem(
		r"(?={%)", Fragment,
		makeGrammarControlFor() + makeGrammarControlIf() + makeGrammarControlElseIf() + makeGrammarControlEnd())
	]


def makeGrammarComments() -> Grammar:
	"""
	Generate the grammar code comments.
	"""

	return makeGrammarCommentStart(
		[GrammarItem(_regexprComment, {"category": "comment"}, makeGrammarCommentStop(FragmentNewElement))])


class Parser(ParserBase):

	def __init__(self, content: str) -> None:
		super().__init__(content,
			grammar=makeGrammarSubstitution() + makeGrammarControl() + makeGrammarComments() + makeGrammarContent(),
			defaultGrammarPost=[GrammarItemSpaces])
