import re
from pathlib import Path

from tools.bzd2.fragments import Fragment, Comment, Type, Name, BracketOpen, BracketClose, Const, Equal, Value, End, ContractOpen, ContractClose, ContractNext
from tools.bzd2.element import Sequence
from tools.bzd2.grammar import Grammar, GrammarItem

_regexprSpace = r"[\s]+"
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
_regexprValue = r"(?P<value>[^\s;\n]+)"


def makeGrammarClass(nestedGrammarItems: Grammar) -> Grammar:
	return [
		GrammarItem(_regexprClass, Type, [
		GrammarItem(_regexprBracketOpen, BracketOpen, nestedGrammarItems + [
		GrammarItem(_regexprBracketClose, BracketClose),
		]),
		])
	]


_grammarContractEntry: Grammar = []
_grammarContractEntry.append(
	GrammarItem(_regexprType, Type, [
	GrammarItem(_regexprValue, Value, [
	GrammarItem(_regexprContractClose, ContractClose),
	GrammarItem(_regexprContractNext, ContractNext, _grammarContractEntry)
	])
	]))

_grammarContract: Grammar = [
	GrammarItem(_regexprContractOpen, ContractOpen,
	_grammarContractEntry + [GrammarItem(_regexprContractClose, ContractClose)]),
]

# variable: [const] type name [= value] [contract];
_grammarVariable: Grammar = [
	GrammarItem(_regexprConst, Const),
	GrammarItem(_regexprType, Type, [
	GrammarItem(
	_regexprName, Name, _grammarContract + [
	GrammarItem(_regexprEqual, Equal, [GrammarItem(_regexprValue, Value, [GrammarItem(_regexprEnd, End)])]),
	GrammarItem(_regexprEnd, End)
	])
	]),
]

_grammar: Grammar = _grammarVariable + makeGrammarClass(_grammarVariable)

class ParsedData:
	def __init__(self, data: Sequence) -> None:
		self.data = data

	def __repr__(self) -> str:
		return str(self.data)

class Parser:

	def __init__(self, path: Path) -> None:
		self.path = path
		self.content = path.read_text()

	def handleError(self, index: int, message: str) -> None:
		contentByLine = self.content.split("\n")
		# Identify the line and column
		current = 0
		line = 0
		for contentLine in contentByLine:
			current += len(contentLine) + 1
			if current > index:
				break
			line += 1
		column = len(contentByLine[line]) - (current - index) + 1

		# Position the cursor
		contentByLine.insert(line + 1, "{}^".format(" " * column))
		contentByLine.insert(line + 2, "{}:{}:{}: error: {}".format(self.path, line + 1, column + 1, message))
		print("\n".join(contentByLine))
		raise Exception()

	def parse(self) -> ParsedData:
		index = 0
		root = Sequence(parent=None, grammar=_grammar)
		element = root.makeElement()
		defaultGrammar = [GrammarItem(_regexprSpace), GrammarItem(_regexprComment)]
		while index < len(self.content):
			m = None
			for item in defaultGrammar + element.getGrammar():
				print(item, item.regexpr)
				m = re.match(item.regexpr, self.content[index:], re.MULTILINE)
				if m:
					print("Match!", item.regexpr, element.getGrammar())
					if item.fragment:
						fragment = item.fragment(index, attrs=m.groupdict())
						element.add(fragment)
						element = fragment.next(element, item.grammar)
					break
			if m is None:
				print(root)
				print(element.getGrammar())
				self.handleError(index, "invalid syntax")
			assert m is not None
			index += m.end()

		print(root)
		print(element.getGrammar())

		print(m)


		return ParsedData(root)
