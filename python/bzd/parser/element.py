import typing

from bzd.parser.fragments import Fragment, Attributes, Attribute, AttributeParser, AttributesSerialize
from bzd.parser.grammar import Grammar
from bzd.parser.context import Context

ElementSerialize = typing.MutableMapping[str, typing.Union[AttributesSerialize, typing.List[typing.Any]]]
SequenceSerialize = typing.List[ElementSerialize]


class Sequence:

	def __init__(self, context: typing.Optional[Context] = None) -> None:
		self.context = context
		self.list: typing.List["Element"] = []

	def __iter__(self) -> typing.Iterator["Element"]:
		for element in self.list:
			if not element.isEmpty():
				yield element

	def __getitem__(self, index: int) -> "Element":
		for i, element in enumerate(self):
			if i == index:
				return element
		raise IndexError("Index ({}) out of bound".format(index))

	def __len__(self) -> int:
		return len([e for e in self.list if not e.isEmpty()])

	def copy(self) -> "SequenceBuilder":
		sequence = SequenceBuilder()
		sequence.list = self.list.copy()
		return sequence

	def __repr__(self) -> str:
		listStr = []
		for element in self.list:
			formattedStr = "- {}".format("\n  ".join(repr(element).split("\n")))
			listStr.append(formattedStr)
		return "\n".join(listStr)

	def serialize(self) -> SequenceSerialize:
		"""
		Serialize a sequence.
		"""
		return [element.serialize() for element in self]

	@staticmethod
	def fromSerialize(sequence: SequenceSerialize, context: typing.Optional[Context] = None) -> "Sequence":
		"""
		Create a sequence from a serialized sequence.
		"""
		s = Sequence(context)
		s.list = [Element.fromSerialize(e, context) for e in sequence]
		return s


T = typing.TypeVar("T", bound="SequenceBuilder")


class SequenceBuilder(Sequence):
	"""
	Represents a sequence that can be built.
	"""

	@staticmethod
	def cast(sequence: Sequence, classType: typing.Type[T]) -> T:
		assert isinstance(classType, type)
		assert issubclass(classType, SequenceBuilder)
		sequence.__class__ = classType
		return typing.cast(T, sequence)

	def merge(self: T, sequence: "Sequence") -> T:
		self.list += sequence.list
		return self

	def addElement(self: T, element: "Element") -> T:
		"""
		Add an element to the sequence.
		"""
		self.list.append(element)
		return self


class SequenceParser(Sequence):
	"""
	This represents a sequence of Elements.
	"""

	def __init__(self, context: typing.Optional[Context], grammar: Grammar,
		parent: typing.Optional["ElementParser"]) -> None:
		super().__init__(context)
		self.grammar = grammar
		self.parent = parent

	def makeElement(self, grammar: typing.Optional[Grammar] = None) -> "ElementParser":
		"""
		Create a new element in the sequence.
		Params:
		- grammar: Optionaly provides a grammar to the new element or reuse existing one.
		"""
		element = ElementParser(context=self.context, grammar=self.grammar if grammar is None else grammar, parent=self)
		self.list.append(element)
		return element

	def getGrammar(self) -> Grammar:
		return self.grammar

	def getElement(self) -> "ElementParser":
		assert self.parent, "This sequence is at the top level."
		return self.parent


class Element:

	def __init__(self, context: typing.Optional[Context] = None) -> None:
		self.context = context
		self.attrs: Attributes = {}
		self.sequences: typing.Dict[str, Sequence] = {}

	@staticmethod
	def fromSerialize(element: ElementSerialize, context: typing.Optional[Context] = None) -> "Element":
		"""
		Create an element from a serialized element.
		"""
		e = Element(context)
		assert isinstance(element["@"], dict)
		e.attrs = {key: Attribute.fromSerialize(attr) for key, attr in element["@"].items()}
		e.sequences = {
			key: Sequence.fromSerialize(sequence, context)  # type: ignore
			for key, sequence in element.items() if key != "@"
		}
		return e

	def isEmpty(self) -> bool:
		"""
		Check wether or not an element is empty. Empty means with no data.
		"""
		return (len(self.attrs.keys()) + len(self.sequences.keys())) == 0

	def getAttrs(self) -> typing.Mapping[str, Attribute]:
		"""
		Return the attributes as a dictionary.
		"""
		return self.attrs

	def getAttr(self, name: str) -> Attribute:
		"""
		Return a specific name attribute value
		"""
		assert name in self.attrs, "Attribute '{}' is not present in '{}'".format(name, str(self))
		return self.attrs[name]

	def getAttrValue(self, name: str, default: typing.Optional[str] = None) -> typing.Optional[str]:
		"""
		Return a specific name attribute value
		"""
		if name in self.attrs:
			return self.attrs[name].value
		return default

	def updateAttrValue(self, name: str, value: str) -> None:
		"""
		Update the value of an existing attribute.
		"""
		self.attrs[name].setValue(value)

	def isAttr(self, name: str) -> bool:
		"""
		Check if an attribute is present
		"""
		if name in self.attrs:
			return True
		return False

	def isNestedSequence(self, kind: str) -> bool:
		"""
		Check if a sequence exists.
		"""
		return bool(kind in self.sequences)

	def getNestedSequence(self, kind: str) -> typing.Optional[Sequence]:
		"""
		Get a current sequence of None if it does not exists.
		"""
		assert kind != "@", "Nested sequence name '@' cannot be used."
		return self.sequences.get(kind, None)

	def getNestedSequences(self) -> typing.Iterator[typing.Tuple[str, Sequence]]:
		"""
		Generator going through all sequences.
		"""
		for kind, sequence in self.sequences.items():
			yield kind, sequence

	def serialize(self) -> ElementSerialize:
		"""
		Serialize an element.
		"""
		data: ElementSerialize = {"@": {key: attr.serialize() for key, attr in self.getAttrs().items()}}
		for kind, sequence in self.getNestedSequences():
			data[kind] = sequence.serialize()
		return data

	def copy(self, ignoreNested: typing.List[str] = []) -> "ElementBuilder":
		"""
		Copy an element to create a new element object.
		"""
		element = ElementBuilder()
		element.attrs = self.attrs.copy()
		element.sequences = {
			kind: sequence.copy()
			for kind, sequence in self.sequences.items() if kind not in ignoreNested
		}
		return element

	def __repr__(self) -> str:
		"""
		Human readable string representation of the element.
		"""
		content = "<Element {}/>".format(" ".join(
			["{}:{}=\"{}\"".format(key, attr.index, attr.value) for key, attr in self.attrs.items()]))

		for kind, sequence in self.sequences.items():
			content += "\n{}:\n{}".format(kind, repr(sequence))

		return content


U = typing.TypeVar("U", bound="ElementBuilder")


class ElementBuilder(Element):

	@staticmethod
	def cast(element: Element, classType: typing.Type[T]) -> T:
		assert isinstance(classType, type)
		assert issubclass(classType, ElementBuilder)
		element.__class__ = classType
		return typing.cast(T, element)

	def addAttr(self: U, key: str, value: str) -> U:
		"""
		Add an attribute to the element.
		"""
		self.attrs[key] = AttributeParser(index=0, value=value)
		return self

	def addAttrs(self: U, data: typing.Dict[str, str]) -> U:
		"""
		Add multiple attributes to an element.
		"""
		for key, value in data.items():
			self.addAttr(key, value)
		return self

	def setNestedSequence(self: U, kind: str, sequence: Sequence) -> U:
		"""
		Set a nested sequence and overwrite exsiting one.
		"""
		self.sequences[kind] = sequence
		return self

	def addElementToNestedSequence(self: U, kind: str, element: Element) -> U:
		"""
		Add an element to a new or existing nested sequence.
		"""
		if kind not in self.sequences:
			self.sequences[kind] = Sequence()
		savedClass = self.__class__
		SequenceBuilder.cast(self.sequences[kind], SequenceBuilder).addElement(element)
		self.__class__ = savedClass
		return self


class ElementParser(Element):

	def __init__(self,
		context: typing.Optional[Context],
		grammar: Grammar,
		parent: typing.Optional[SequenceParser] = None) -> None:
		super().__init__(context)
		self.grammar = grammar
		self.parent = parent

	@property
	def isRoot(self) -> bool:
		return self.parent is None

	def add(self, fragment: Fragment) -> None:
		"""
		Add a new fragment to this element.
		"""
		fragment.merge(self.attrs)

	def setGrammar(self, grammar: Grammar) -> None:
		"""
		Update the grammar of the current element.
		"""
		self.grammar = grammar

	def getGrammar(self) -> Grammar:
		return self.grammar

	def makeElement(self, kind: str, grammar: Grammar) -> "ElementParser":
		if kind not in self.sequences:
			self.sequences[kind] = SequenceParser(context=self.context, grammar=grammar, parent=self)
		return typing.cast(SequenceParser, self.sequences[kind]).makeElement()

	def getSequence(self) -> SequenceParser:
		assert not self.isRoot, "reached root element"
		assert isinstance(self.parent,
			SequenceParser), "parent must be a sequence, instead {}".format(type(self.parent))
		return self.parent
