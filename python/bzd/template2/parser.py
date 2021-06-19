import typing

from bzd.parser.parser import Parser as ParserBase
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.fragments import Fragment, FragmentNestedStart, FragmentNestedStopNewElement, FragmentNewElement, FragmentParentElement
from bzd.parser.element import Element

# Match all remaining content
_regexprContent = r"(?P<content>((?!{[{%#]).)+?)(?={[{%#])"
_regexprContentStripRight = r"(?P<content>((?!{[{%#]).)*?)\s*(?={[{%#]-)"
_regexprContentEndOfFile = r"(?P<content>[\s\S]+?)(?=$)"
# Match an identifier
_regexprIdentifier = r"(?P<name>([a-zA-Z_\-0-9]+))"
# Match condition
_regexprCondition = r"(?P<condition>(.+?(?=-?%})))"
# Match comment
_regexprComment = r"(?P<comment>(.+?(?=-?#})))"
# Number
_regexprNumber = r"(?P<value>-?[0-9]+(?:\.[0-9]*)?)"
# String
_regexprString = r"\"(?P<value>.*?(?<!\\))\""


def makeRegexprName(name: str) -> str:
	"""
	Match a name.
	"""
	return r"(?P<" + name + r">([a-zA-Z_\-0-9\.]+))"


def makeGrammarContent() -> Grammar:
	"""
	Generate a grammar for the raw content.
	"""

	class FragmentContent(FragmentNewElement):
		default = {"category": "content"}

	return [
		GrammarItem(_regexprContentStripRight, FragmentContent),
		GrammarItem(_regexprContent, FragmentContent),
		GrammarItem(_regexprContentEndOfFile, FragmentContent)
	]


def makeGrammarSubstitutionStart(grammar: Grammar) -> Grammar:
	return [GrammarItem(r"{{-?", Fragment, grammar)]


def makeGrammarControlStart(keyword: str, grammar: Grammar) -> Grammar:
	return [
		GrammarItem(r"{%-?\s*" + keyword, Fragment, grammar),
	]


def makeGrammarCommentStart(grammar: Grammar) -> Grammar:
	return [
		GrammarItem(r"{#-?", Fragment, grammar),
	]


def makeGrammarSubstitutionStop(fragment: typing.Type[Fragment]) -> Grammar:
	return [
		GrammarItem(r"(?=}})", Fragment, [GrammarItem(r"}}", fragment)]),
		GrammarItem(r"(?=-}})", Fragment, [GrammarItem(r"-}}\s*", fragment)]),
	]


def makeGrammarControlStop(fragment: typing.Type[Fragment],
	grammar: typing.Optional[typing.Union[Grammar, str]] = None) -> Grammar:
	return [
		GrammarItem(r"(?=%})", Fragment, [GrammarItem(r"%}([ \t]*\n|[. \t]*$)?", fragment, grammar)]),
		GrammarItem(r"(?=-%})", Fragment, [GrammarItem(r"-%}\s*", fragment, grammar)]),
	]


def makeGrammarCommentStop(fragment: typing.Type[Fragment]) -> Grammar:
	return [
		GrammarItem(r"(?=#})", Fragment, [GrammarItem(r"#}([ \t]*\n|[. \t]*$)?", fragment)]),
		GrammarItem(r"(?=-#})", Fragment, [GrammarItem(r"-#}\s*", fragment)]),
	]


def makeGrammarValue(fragment: typing.Dict[str, str], grammar: Grammar) -> Grammar:
	"""
	A value is either a name, a number or a string.
	"""
	return [
		GrammarItem(_regexprNumber, dict(fragment, type="number"), grammar),
		GrammarItem(_regexprString, dict(fragment, type="string"), grammar),
		GrammarItem(makeRegexprName("value"), dict(fragment, type="name"), grammar),
	]


def makeGrammarSymbol(fragment: typing.Dict[str, str], grammar: Grammar) -> Grammar:
	"""
	Generate a grammar for a symbol.
	A symbol is a entity that must be resolved from substitution.
	"""

	class ArgumentStart(FragmentNestedStart):
		nestedName = "argument"

	return [
		GrammarItem(makeRegexprName("name"), fragment, [
		GrammarItem(
		r"\(", ArgumentStart,
		makeGrammarValue({},
		[GrammarItem(r",", FragmentNewElement),
		GrammarItem(r"\)", FragmentParentElement, grammar)]) + [GrammarItem(r"\)", FragmentParentElement, grammar)])
		] + grammar)
	]


def makeGrammarSubstitution() -> Grammar:
	"""
	Generate a grammar for substitution blocks.
	"""

	class PipeStart(FragmentNestedStart):
		nestedName = "pipe"

	return makeGrammarSubstitutionStart(
		makeGrammarSymbol({"category": "substitution"}, [
		GrammarItem(r"\|", PipeStart, [
		GrammarItem(makeRegexprName("name"), Fragment,
		[GrammarItem(r"\|", FragmentNewElement),
		GrammarItem(None, FragmentParentElement)])
		])
		] + makeGrammarSubstitutionStop(FragmentNewElement)) + makeGrammarSubstitutionStop(FragmentNewElement))


def makeGrammarControlInclude() -> Grammar:
	"""
	Generate the grammar for the include block.
	It matches the following:
		include value %}
	"""
	return makeGrammarControlStart(
		r"include", makeGrammarValue({"category": "include"}, makeGrammarControlStop(FragmentNewElement)))


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

	return makeGrammarControlStart(r"for", [
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
	return makeGrammarControlStart(r"if", [
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
		makeGrammarControlStart(r"elif",
		[GrammarItem(_regexprCondition, {"category": "else"}, makeGrammarControlStop(FragmentNestedStart, "root"))]) +
		makeGrammarControlStart(r"else", makeGrammarControlStop(ElseFragment, "root")))
	]


def makeGrammarControlMacro() -> Grammar:
	"""
	Generate the grammar for a macro block.
	It matches the following:
		macro name(arg1, arg2, ...) %}
	"""

	class ArgumentStart(FragmentNestedStart):
		nestedName = "argument"
		default = {"category": "macro"}

	return makeGrammarControlStart(r"macro", [
		GrammarItem(_regexprIdentifier + r"\(", ArgumentStart, [
		GrammarItem(makeRegexprName("name"), Fragment, [
		GrammarItem(r",", FragmentNewElement),
		GrammarItem(r"\)", FragmentParentElement, makeGrammarControlStop(FragmentNestedStart, "root"))
		]),
		GrammarItem(r"\)", FragmentParentElement, makeGrammarControlStop(FragmentNestedStart, "root"))
		])
	])


def makeGrammarControlEnd() -> Grammar:
	"""
	Generate the grammar for the end control block.
	"""

	class FragmentEndElement(FragmentNewElement):
		default = {"category": "end"}

	return [
		GrammarItem(
		r"(?={%-?\s*end)", FragmentParentElement,
		makeGrammarControlStart(r"end",
		[GrammarItem(None, FragmentNewElement, makeGrammarControlStop(FragmentEndElement))]))
	]


def makeGrammarControl() -> Grammar:
	"""
	Generate the grammar for all control blocks.
	"""

	return [
		GrammarItem(
		r"(?={%)", Fragment,
		makeGrammarControlFor() + makeGrammarControlIf() + makeGrammarControlElseIf() + makeGrammarControlMacro() +
		makeGrammarControlInclude() + makeGrammarControlEnd())
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
