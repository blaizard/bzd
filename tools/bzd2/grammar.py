from pathlib import Path

from bzd.parser.parser import Parser as ParserBase
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.fragments import Fragment, FragmentNestedStart, FragmentNestedStop, FragmentNewElement, FragmentParentElement
from bzd.parser.element import Element


class ContractStart(FragmentNestedStart):
	nestedName = "contract"


_regexprComment = r"/\*(?P<comment>([\s\S]*?))\*/"
_regexprClass = r"(?P<kind>(:?interface|struct))\s+(?P<name>\S+)"
_regexprConst = r"const\b"
_regexprClassType = r"(?P<kind>(:?interface|struct))\b"
_regexprType = r"(?P<kind>(?!const|interface|struct)[^\s]+)\b"
_regexprName = r"(?P<name>\S+)\b"
_regexprBracketOpen = r"{"
_regexprBracketClose = r"}"
_regexprContractOpen = r"\["
_regexprContractClose = r"\]"
_regexprContractNext = r","
_regexprEqual = r"="
_regexprEnd = r";"
_regexprValue = r"(?P<value>-?[0-9]+)"


def makeGrammarClass(nestedGrammarItems: Grammar) -> Grammar:
	return [
		GrammarItem(_regexprClass, Fragment, [
		GrammarItem(_regexprBracketOpen, FragmentNestedStart, nestedGrammarItems + [
		GrammarItem(_regexprBracketClose, FragmentNestedStop),
		]),
		])
	]


_grammarContractEntry: Grammar = []
_grammarContractEntry.append(
	GrammarItem(_regexprType, Fragment, [
	GrammarItem(_regexprValue, Fragment, [
	GrammarItem(_regexprContractClose, FragmentParentElement),
	GrammarItem(_regexprContractNext, FragmentNewElement, _grammarContractEntry)
	])
	]))

_grammarContract: Grammar = [
	GrammarItem(_regexprContractOpen, ContractStart,
	_grammarContractEntry + [GrammarItem(_regexprContractClose, FragmentParentElement)]),
]

# variable: [const] type name [= value] [contract];
_grammarVariable: Grammar = [
	GrammarItem(_regexprConst, "const"),
	GrammarItem(_regexprType, Fragment, [
	GrammarItem(
	_regexprName, Fragment, _grammarContract + [
	GrammarItem(_regexprEqual, Fragment,
	[GrammarItem(_regexprValue, Fragment, [GrammarItem(_regexprEnd, FragmentNewElement)])]),
	GrammarItem(_regexprEnd, FragmentNewElement)
	])
	]),
]

_grammar: Grammar = _grammarVariable + makeGrammarClass(_grammarVariable)


class Parser(ParserBase):

	def __init__(self, path: Path) -> None:
		super().__init__(path, grammar=_grammar, defaultGrammar=[GrammarItemSpaces, GrammarItem(_regexprComment)])
