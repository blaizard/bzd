import re
from pathlib import Path
from typing import Any, Iterator, MutableMapping, Type, Optional

from bzd.parser.context import Context
from bzd.parser.element import SequenceParser
from bzd.parser.grammar import Grammar, GrammarItem, GrammarItemSpaces
from bzd.parser.error import Error


class Parser:

	def __init__(self,
		content: str,
		grammar: Grammar,
		defaultGrammarPre: Grammar = [],
		defaultGrammarPost: Grammar = []) -> None:

		self.grammar = grammar
		self.defaultGrammarPre = defaultGrammarPre
		self.defaultGrammarPost = defaultGrammarPost

		# Save the context
		self.context = Context(content=content)

	@classmethod
	def fromPath(cls: Type["Parser"], path: Path, *args: Any, **kwargs: Any) -> "Parser":
		"""
		Make a parser instance from a file path using its text as a content.
		"""

		content = path.read_text()
		parser = cls(content, *args, **kwargs)
		parser.context.setPath(path)

		return parser

	def iterateGrammar(self, grammar: Grammar) -> Iterator[GrammarItem]:
		"""
		Helper to loop through the grammars.
		"""

		for item in grammar:
			if isinstance(item, list):
				yield from self.iterateGrammar(item)
			elif isinstance(item, GrammarItem):
				yield item
			elif callable(item):
				yield from self.iterateGrammar(item())
			else:
				assert False, "Grammar item must be of type Grammar or GrammarItem, received: {}".format(type(item))

	def getGrammar(self, checkpoints: MutableMapping[str, Grammar], item: GrammarItem) -> Optional[Grammar]:
		"""
		Get the grammar from a grammar item and update the checkpoint if needed.
		"""
		# Grammar link to a checkpoint
		if isinstance(item.grammar, str):
			assert item.grammar in checkpoints, "Unknown checkpoint '{}', ensure the parser discovered it before referencing it.".format(
				item.grammar)
			assert item.checkpoint is None, "A grammar item referencing to a checkpoint cannot set a checkpoint."
			return checkpoints[item.grammar]

		# Register the checkpoint
		if item.checkpoint is not None:
			assert item.grammar, "Grammar must be set for a checkpoint."
			checkpoints[item.checkpoint] = item.grammar

		return item.grammar

	def parse(self) -> SequenceParser:
		"""
		Parse the content using the provided grammar.
		"""

		index = 0
		root = SequenceParser(context=self.context, parent=None, grammar=self.grammar)
		element = root.makeElement()
		checkpoints: MutableMapping[str, Grammar] = {"root": self.grammar}

		# Keep a reference to the content
		assert self.context.content
		content: str = self.context.content

		try:
			while index < len(content):
				m = None
				for item in self.iterateGrammar(self.defaultGrammarPre + element.getGrammar() +
					self.defaultGrammarPost):
					m = re.match(item.regexpr, content[index:])
					if m:
						if item.fragment:
							fragment = item.fragment(index, index + m.end(), attrs=m.groupdict())
							element.add(fragment)
							grammar = self.getGrammar(checkpoints=checkpoints, item=item)
							element = fragment.next(element, grammar)
						break
				if m is None:
					raise Exception("Invalid syntax.")
				assert m is not None
				index += m.end()

		except Exception as e:

			# Uncomment for debug
			print("**** debug ****\n", root)
			for item in self.iterateGrammar(self.defaultGrammarPre + element.getGrammar() + self.defaultGrammarPost):
				print(item.regexpr)

			Error.handle(context=self.context, index=index, end=index, message=str(e))

		return root
