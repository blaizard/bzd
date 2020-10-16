import re
from pathlib import Path
from typing import Iterator

from bzd.parser.element import Sequence
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces


class ParsedData:

	def __init__(self, data: Sequence) -> None:
		self.data = data

	def __repr__(self) -> str:
		return str(self.data)


class Parser:

	def __init__(self, path: Path, grammar: Grammar, defaultGrammar: Grammar = [GrammarItemSpaces]) -> None:
		self.path = path
		self.grammar = grammar
		self.defaultGrammar = defaultGrammar
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

	def iterateGrammar(self, grammar: Grammar) -> Iterator[GrammarItem]:
		for item in grammar:
			if isinstance(item, list):
				yield from self.iterateGrammar(item)
			else:
				yield item

	def parse(self) -> ParsedData:
		index = 0
		root = Sequence(parent=None, grammar=self.grammar)
		element = root.makeElement()
		while index < len(self.content):
			m = None
			for item in self.iterateGrammar(self.defaultGrammar + element.getGrammar()):
				m = re.match(item.regexpr, self.content[index:], re.MULTILINE)
				if m:
					if item.fragment:
						fragment = item.fragment(index, attrs=m.groupdict())
						element.add(fragment)
						element = fragment.next(element, item.grammar)
					break
			if m is None:
				self.handleError(index, "invalid syntax")
			assert m is not None
			index += m.end()

		return ParsedData(root)
