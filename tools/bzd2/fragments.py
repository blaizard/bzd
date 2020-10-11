from typing import Dict, TYPE_CHECKING

if TYPE_CHECKING:
	from tools.bzd2.element import Element
	from tools.bzd2.grammar import Grammar

Attributes = Dict[str, str]


class Fragment:
	"""
	A fragment is the smallest typed entity, that optionally contains attributes.
	It describes any entity discovered by the parser. 
	"""

	default: Dict[str, str] = {}

	def __init__(self, index: int, attrs: Attributes) -> None:
		self.index = index
		self.attrs = self.default.copy()
		self.attrs.update(attrs)

	def merge(self, attrs: Attributes) -> None:
		"""
		Merge current attributes with existing ones
		"""
		for key, value in self.attrs.items():
			assert key not in attrs, "Attribute '{}' already set".format(key)
			attrs[key] = value

	def next(self, element: "Element", grammar: "Grammar") -> "Element":
		"""
		Returns the next element for the next entity.
		This is where elements can be terminated and new ones can be generated.
		"""
		if grammar is not None:
			element.setGrammar(grammar)
		return element

	def __repr__(self) -> str:
		return "<{}:{} {}/>".format(self.__class__.__name__, self.index,
			" ".join(["{}='{}'".format(key, value) for key, value in self.attrs.items()]))


class Comment(Fragment):
	pass


class Type(Fragment):
	pass


class Const(Fragment):

	default: Dict[str, str] = {"const": ""}


class Equal(Fragment):
	pass


class Name(Fragment):
	pass


class Value(Fragment):
	pass


class End(Fragment):

	def next(self, element: "Element", grammar: "Grammar") -> "Element":
		return element.getSequence().makeElement()


class BracketOpen(Fragment):

	def next(self, element: "Element", grammar: "Grammar") -> "Element":
		return element.makeElement("children", grammar)


class BracketClose(Fragment):

	def next(self, element: "Element", grammar: "Grammar") -> "Element":
		return element.getSequence().getElement().getSequence().makeElement()


class ContractOpen(Fragment):

	def next(self, element: "Element", grammar: "Grammar") -> "Element":
		return element.makeElement("contract", grammar)


class ContractClose(Fragment):

	def next(self, element: "Element", grammar: "Grammar") -> "Element":
		return element.getSequence().getElement()


class ContractNext(Fragment):

	def next(self, element: "Element", grammar: "Grammar") -> "Element":
		return element.getSequence().makeElement()
