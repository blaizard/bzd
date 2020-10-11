import typing

from tools.bzd2.fragments import Fragment, Attributes
from tools.bzd2.grammar import Grammar


class Sequence:
	"""
	This represents a sequence of Elements.
	"""

	def __init__(self, grammar: Grammar, parent: "Element") -> None:
		self.grammar = grammar
		self.parent = parent
		self.list: typing.List["Element"] = []

	def makeElement(self) -> "Element":
		element = Element(grammar=self.grammar, parent=self)
		self.list.append(element)
		return element

	def getGrammar(self) -> Grammar:
		return self.grammar

	def getElement(self) -> "Element":
		return self.parent

	def __repr__(self) -> str:
		listStr = []
		for element in self.list:
			formattedStr = "- {}".format("\n  ".join(repr(element).split("\n")))
			listStr.append(formattedStr)
		return "\n".join(listStr)


class Element:

	def __init__(self, grammar: Grammar, parent: typing.Optional[Sequence] = None) -> None:
		self.grammar = grammar
		self.parent = parent
		self.attrs: Attributes = {}
		self.sequences: typing.Dict[str, Sequence] = {}

	def empty(self):
		"""
		Check wether or not an element is empty. Empty means with no data.
		"""
		return (len(self.attrs.keys()) + len(self.sequences.keys())) == 0

	def add(self, fragment: Fragment) -> None:
		"""
		Add a new fragment to this element.
		"""
		fragment.merge(self.attrs)

	def setGrammar(self, grammar: Grammar) -> None:
		"""
	def getAttrs(self) -> Attributes:
		return self.attrs

		Update the grammar of the current element.
		"""
		self.grammar = grammar

	def getGrammar(self) -> Grammar:
		return self.grammar

	def makeElement(self, kind: str, grammar: Grammar) -> "Element":
		if kind not in self.sequences:
			self.sequences[kind] = Sequence(grammar=grammar, parent=self)
		return self.sequences[kind].makeElement()

	def getSequence(self) -> Sequence:
		assert self.parent is not None, "reached parent element"
		assert isinstance(self.parent, Sequence), "parent must be a sequence, instead {}".format(type(self.parent))
		return self.parent

	def __repr__(self) -> str:
		content = "<Element {}/>".format(" ".join(["{}=\"{}\"".format(key, value)
			for key, value in self.attrs.items()]))

		for kind, sequence in self.sequences.items():
			content += "\n{}:\n{}".format(kind, repr(sequence))

		return content
