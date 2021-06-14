import re
from typing import cast, Dict, Mapping, MutableMapping, Optional, Union, TYPE_CHECKING

if TYPE_CHECKING:
	from bzd.parser.element import ElementParser
	from bzd.parser.grammar import Grammar

AttributeSerialize = MutableMapping[str, Union[int, str]]
AttributesSerialize = Dict[str, AttributeSerialize]


class Attribute:

	def __init__(self, data: AttributeSerialize) -> None:
		self.data = data

	@property
	def index(self) -> int:
		return cast(int, self.data["i"])

	@property
	def value(self) -> str:
		return cast(str, self.data["v"])

	def serialize(self) -> AttributeSerialize:
		return self.data

	@staticmethod
	def fromSerialize(attribute: AttributeSerialize) -> "Attribute":
		"""
		Create an attribute from a serialized attribute.
		"""
		return Attribute(attribute)


class AttributeParser(Attribute):

	def __init__(self, index: int, value: str) -> None:
		super().__init__({"v": value, "i": index})


Attributes = Dict[str, Attribute]


class Fragment:
	"""
	A fragment is the smallest typed entity, that optionally contains attributes.
	It describes any entity discovered by the parser. 
	"""

	default: Dict[str, str] = {}

	def __init__(self, index: int, attrs: Mapping[str, str]) -> None:
		self.index = index
		self.attrs = self.default.copy()
		self.attrs.update(attrs)
		self.process()

	def merge(self, attrs: Attributes) -> None:
		"""
		Merge current attributes with existing ones
		"""
		for key, value in self.attrs.items():
			assert key not in attrs, "Attribute '{}' already set".format(key)
			attrs[key] = AttributeParser(index=self.index, value=value)

	def process(self) -> None:
		"""
		Post process a value discovered.
		"""
		pass

	def next(self, element: "ElementParser", grammar: Optional["Grammar"]) -> "ElementParser":
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


class FragmentComment(Fragment):
	"""
	Helper fragment for comments.
	"""

	def merge(self, attrs: Attributes) -> None:
		for key, value in self.attrs.items():

			# Remove empty lines at the begining and end
			value = re.sub(r"^(\s*\n)+", "", value)
			value = re.sub(r"(\n\s*)+$", "", value)

			# Remove common prefix and trailing spaces
			minTrailingSpaces = min([len(s) - len(s.lstrip(" ")) for s in value.split("\n")])
			updatedValue = "\n".join([s[minTrailingSpaces:].rstrip() for s in value.split("\n")])

			# Append the comments
			if key in attrs:
				attrs[key] = AttributeParser(index=attrs[key].index,
					value=attrs[key].value + "\n\n{}".format(updatedValue))
			else:
				attrs[key] = AttributeParser(index=self.index, value=updatedValue)


class FragmentNewElement(Fragment):
	"""
	Helper fragment to create a new element.
	"""

	def next(self, element: "ElementParser", grammar: Optional["Grammar"]) -> "ElementParser":
		return element.getSequence().makeElement(grammar = grammar)


class FragmentParentElement(Fragment):
	"""
	Helper fragment to continue on the parent element.
	"""

	def next(self, element: "ElementParser", grammar: Optional["Grammar"]) -> "ElementParser":
		element = element.getSequence().getElement()
		#if grammar is not None:
		#	element.setGrammar(grammar)
		return element


class FragmentNestedStart(Fragment):
	"""
	Helper fragment to start a nested sequence.
	"""

	nestedName = "nested"

	def next(self, element: "ElementParser", grammar: Optional["Grammar"]) -> "ElementParser":
		assert grammar is not None, "Grammar must be set"
		return element.makeElement(self.nestedName, grammar)


class FragmentNestedStopNewElement(Fragment):
	"""
	Helper fragment to stop a nested sequence and create a new element.
	"""

	def next(self, element: "ElementParser", grammar: Optional["Grammar"]) -> "ElementParser":
		return element.getSequence().getElement().getSequence().makeElement(grammar = grammar)
