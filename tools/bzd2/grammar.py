import re
from pathlib import Path

from bzd.parser.parser import Parser as ParserBase
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.fragments import Fragment, FragmentNestedStart, FragmentNestedStop, FragmentNewElement, FragmentParentElement, FragmentComment
from bzd.parser.element import Element

# Match: interface, struct
_regexprClass = r"(?P<type>(:?interface|struct))"
# Match: any type expect protected types
_regexprType = r"(?P<type>(?!const|interface|struct)[0-9a-zA-Z_]+)"
# Match name
_regexprName = r"(?P<name>[0-9a-zA-Z_]+)\b"
# Match: "string", 12, -45, 5.1854
_regexprValue = r"(?P<value>\".*?(?<!\\)\"|-?[0-9]+(?:\.[0-9]*)?)"


class FragmentBlockComment(FragmentComment):

	def process(self) -> None:
		assert "comment" in self.attrs, "Missing comment attribute."
		self.attrs["comment"] = re.sub(re.compile("^\ {0,2}\*+", re.MULTILINE), "", self.attrs["comment"])


def makeGrammarClass(nestedGrammar: Grammar) -> Grammar:
	"""
	Generate a grammar for Class., it accepst the following format:
	(interface|struct) name {
		nestedGrammar
	}

	Nested type elements are included under `nested`.
	"""

	return [
		GrammarItem(_regexprClass, {"category": "class"}, [
		GrammarItem(_regexprName, Fragment, [
		GrammarItem(r"{", FragmentNestedStart, [
		nestedGrammar,
		GrammarItem(r"}", FragmentNestedStop),
		]),
		])
		])
	]


def makeGrammarType(nextGrammar: Grammar) -> Grammar:
	"""
	Generate a grammar for Type, it accepts the following format:
	Type = Type1[<Type, Type, ...>]

	Nested type elements are included under `template`.
	"""

	class TemplateStart(FragmentNestedStart):
		nestedName = "template"

	grammar: Grammar = []
	grammar.append(
		GrammarItem(_regexprType, Fragment, [
		GrammarItem(r"<", TemplateStart, [grammar]),
		GrammarItem(r",", FragmentNewElement),
		GrammarItem(r">", FragmentParentElement), nextGrammar
		]))
	return grammar


def makeGrammarContracts() -> Grammar:
	"""
	Generate a grammar for Contracts, it accepts the following format:
	[Type1 [= value], Type2 [= value], ...]

	Nested type elements are included under `contracts`.
	"""

	class ContractStart(FragmentNestedStart):
		nestedName = "contract"

	return [
		GrammarItem(r"\[", ContractStart, [
		GrammarItem(_regexprType, Fragment, [
		GrammarItem(r"=", Fragment, [
		GrammarItem(_regexprValue, Fragment),
		GrammarItem(r",", FragmentNewElement),
		GrammarItem(r"\]", FragmentParentElement),
		]),
		GrammarItem(r",", FragmentNewElement),
		GrammarItem(r"\]", FragmentParentElement),
		])
		]),
	]


def makeGrammarVariable() -> Grammar:
	"""
	Generate a grammar for Variables, it accepts the following format:
	[const] type name [= value] [contract];
	"""
	return [
		GrammarItem(r"const", {"const": ""}),
	] + makeGrammarType([
		GrammarItem(_regexprName, {"category": "variable"}, [
		makeGrammarContracts(),
		GrammarItem(r"=", Fragment,
		[GrammarItem(_regexprValue, Fragment,
		[makeGrammarContracts(), GrammarItem(r";", FragmentNewElement)])]),
		GrammarItem(r";", FragmentNewElement)
		])
	])


# Comments allowed by the grammar
_grammarComments = [
	GrammarItem(r"/\*(?P<comment>([\s\S]*?))\*/", FragmentBlockComment),
	GrammarItem(r"//(?P<comment>[^\n]*)", FragmentComment)
]


class Parser(ParserBase):

	def __init__(self, path: Path) -> None:
		super().__init__(path,
			grammar=makeGrammarVariable() + makeGrammarClass(makeGrammarVariable()),
			defaultGrammar=[GrammarItemSpaces] + _grammarComments)
