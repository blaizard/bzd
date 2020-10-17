import re
from pathlib import Path
from typing import Iterator

from bzd.parser.element import SequenceParser
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.error import handleError


class Parser:

	def __init__(self, path: Path, grammar: Grammar, defaultGrammar: Grammar = [GrammarItemSpaces]) -> None:
		self.path = path
		self.grammar = grammar
		self.defaultGrammar = defaultGrammar
		self.content = path.read_text()

	def handleError(self, index: int, message: str) -> None:
		handleError(parser=self, index=index, message=message)

	def iterateGrammar(self, grammar: Grammar) -> Iterator[GrammarItem]:
		for item in grammar:
			if isinstance(item, list):
				yield from self.iterateGrammar(item)
			else:
				assert isinstance(item,
					GrammarItem), "Grammar item must be of type Grammar or GrammarItem, received: {}".format(type(item))
				yield item

	def parse(self) -> SequenceParser:
		index = 0
		root = SequenceParser(parser=self, parent=None, grammar=self.grammar)
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
				self.handleError(index=index, message="Invalid syntax.")
			assert m is not None
			index += m.end()

		return root
