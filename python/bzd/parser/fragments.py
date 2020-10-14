from typing import Dict, Optional, TYPE_CHECKING

if TYPE_CHECKING:
	from bzd.parser.element import Element
	from bzd.parser.grammar import Grammar

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

	def next(self, element: "Element", grammar: Optional["Grammar"]) -> "Element":
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


class FragmentNewElement(Fragment):
	"""
	Helper fragment to create a new element.
	"""

	def next(self, element: "Element", grammar: Optional["Grammar"]) -> "Element":
		return element.getSequence().makeElement()


class FragmentParentElement(Fragment):
	"""
	Helper fragment to continue on the parent element.
	"""

	def next(self, element: "Element", grammar: Optional["Grammar"]) -> "Element":
		return element.getSequence().getElement()


class FragmentNestedStart(Fragment):
	"""
	Helper fragment to start a nested sequence.
	"""

	nestedName = "nested"

	def next(self, element: "Element", grammar: Optional["Grammar"]) -> "Element":
		assert grammar is not None, "Grammar must be set"
		return element.makeElement(self.nestedName, grammar)


class FragmentNestedStop(Fragment):
	"""
	Helper fragment to stop a nested sequence.
	"""

	def next(self, element: "Element", grammar: Optional["Grammar"]) -> "Element":
		return element.getSequence().getElement().getSequence().makeElement()
