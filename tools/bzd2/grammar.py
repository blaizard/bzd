from pathlib import Path

from bzd.parser.parser import Parser as ParserBase
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.fragments import Fragment, FragmentNestedStart, FragmentNestedStop, FragmentNewElement, FragmentParentElement, FragmentComment
from bzd.parser.element import Element

# Match: interface, struct
_regexprClass = r"(?P<type>(:?interface|struct))"
# Match: any type expect protected types
_regexprType = r"(?P<kind>(?!const|interface|struct)[^\s<>,]+)"
# Match name
_regexprName = r"(?P<name>[0-9a-zA-Z_-]+)\b"
# Match: "string", 12, -45, 5.1854
_regexprValue = r"(?P<value>\".*?(?<!\\)\"|-?[0-9]+(?:\.[0-9]*)?)"


def makeGrammarClass(nestedGrammar: Grammar) -> Grammar:
	"""
	Generate a grammar for Class., it accepst the following format:
	(interface|struct) name {
		nestedGrammar
	}

	Nested type elements are included under `nested`.
	"""

	return [
		GrammarItem(_regexprClass, Fragment, [
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
	grammar.append(GrammarItem(_regexprType, Fragment, [
		GrammarItem(r"<", TemplateStart, [
			grammar
		]),
		GrammarItem(r",", FragmentNewElement),
		GrammarItem(r">", FragmentParentElement),
		nextGrammar
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

	_grammarContractEntry: Grammar = []
	_grammarContractEntry.append(
		GrammarItem(_regexprType, Fragment, [
			GrammarItem(_regexprValue, Fragment, [
				GrammarItem(r"\]", FragmentParentElement),
				GrammarItem(r",", FragmentNewElement, _grammarContractEntry)
			])
		])
	)

	return [
		GrammarItem(r"\[", ContractStart,
		[_grammarContractEntry, GrammarItem(r"\]", FragmentParentElement)]),
	]

# variable: [const] type name [= value] [contract];
_grammarVariable: Grammar = [
	GrammarItem(r"const", "const"),
] + makeGrammarType([
	GrammarItem(
	_regexprName, Fragment, [makeGrammarContracts(),
	GrammarItem(r"=", Fragment,
	[GrammarItem(_regexprValue, Fragment, [GrammarItem(r";", FragmentNewElement)])]),
	GrammarItem(r";", FragmentNewElement)
	])
])

_grammar: Grammar = _grammarVariable + makeGrammarClass(_grammarVariable)

_grammarComments = [
	GrammarItem(r"/\*(?P<comment>([\s\S]*?))\*/", FragmentComment),
	GrammarItem(r"//(?P<comment>[^\n]*)", FragmentComment)
]


class Parser(ParserBase):

	def __init__(self, path: Path) -> None:
		super().__init__(path, grammar=_grammar, defaultGrammar=[GrammarItemSpaces] + _grammarComments)
