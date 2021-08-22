import typing
import copy
import sys
from dataclasses import dataclass

from bzd.utils.dict import isEqual

from bzd.parser.fragments import Fragment, Attributes, Attribute, AttributeParser, AttributesSerialize, IGNORE_INDEX_VALUE
from bzd.parser.grammar import Grammar
from bzd.parser.context import Context

ElementSerialize = typing.MutableMapping[str, typing.Union[AttributesSerialize, typing.List[typing.Any]]]
SequenceSerialize = typing.List[ElementSerialize]


class Sequence:

	def __init__(self, context: typing.Optional[Context] = None) -> None:
		self.context = Context() if context is None else context
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

	def __repr__(self) -> str:
		listStr = []
		for element in self.list:
			formattedStr = "- {}".format("\n  ".join(repr(element).split("\n")))
			listStr.append(formattedStr)
		return "\n".join(listStr)

	def serialize(self, ignoreContext: bool = False) -> SequenceSerialize:
		"""
		Serialize a sequence.
		"""
		return [element.serialize(ignoreContext=ignoreContext) for element in self]

	@staticmethod
	def fromSerialize(sequence: SequenceSerialize, context: typing.Optional[Context] = None) -> "Sequence":
		"""
		Create a sequence from a serialized sequence.
		"""
		s = SequenceBuilder(context)
		for e in sequence:
			s.pushBackElement(Element.fromSerialize(e))
		return s


T = typing.TypeVar("T", bound="SequenceBuilder")


class SequenceBuilder(Sequence):
	"""
	Represents a sequence that can be built.
	"""

	@staticmethod
	def cast(sequence: Sequence, classType: typing.Type[T]) -> T:
		"""
		Return a copy of the sequence casted to a given type.
		"""
		assert isinstance(sequence, Sequence)
		assert isinstance(classType, type)
		assert issubclass(classType, SequenceBuilder)
		copiedSequence = copy.copy(sequence)
		copiedSequence.__class__ = classType
		return typing.cast(T, copiedSequence)

	def merge(self: T, sequence: "Sequence") -> T:
		for element in sequence.list:
			self.pushBackElement(element)
		return self

	def pushBackElement(self: T, element: "Element") -> T:
		"""
		Add an element to the sequence at the end of the list.
		"""
		element.context.setParent(self)
		self.list.append(element)
		return self

	def pushFrontElement(self: T, element: "Element") -> T:
		"""
		Add an element to the sequence at the begining of the list.
		"""
		element.context.setParent(self)
		self.list.insert(0, element)
		return self

	def removeElement(self: T, element: "Element") -> T:
		"""
		Remove an element from the sequence.
		"""
		element.context.clear()
		self.list.remove(element)
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
		element = ElementParser(context=Context(parent=self),
			grammar=self.grammar if grammar is None else grammar,
			parent=self)
		self.list.append(element)
		return element

	def getGrammar(self) -> Grammar:
		return self.grammar

	def getElement(self) -> "ElementParser":
		assert self.parent, "This sequence is at the top level."
		return self.parent


class Element:

	def __init__(self, context: typing.Optional[Context] = None) -> None:
		self.context = Context() if context is None else context
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
			key: Sequence.fromSerialize(sequence, Context(parent=e))  # type: ignore
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

	def getNestedSequenceAssert(self, kind: str) -> Sequence:
		"""
		Get a current sequence of None if it does not exists.
		"""
		maybeSequence = self.getNestedSequence(kind)
		assert maybeSequence, "Sequence must be non-null."
		return maybeSequence

	def getNestedSequences(self) -> typing.Iterator[typing.Tuple[str, Sequence]]:
		"""
		Generator going through all sequences.
		"""
		for kind, sequence in self.sequences.items():
			yield kind, sequence

	def serialize(self, ignoreContext: bool = False) -> ElementSerialize:
		"""
		Serialize an element.
		"""
		data: ElementSerialize = {
			"@": {key: attr.serialize(ignoreContext=ignoreContext)
			for key, attr in self.getAttrs().items()}
		}
		for kind, sequence in self.getNestedSequences():
			data[kind] = sequence.serialize(ignoreContext=ignoreContext)
		return data

	def copy(self, ignoreNested: typing.List[str] = []) -> "ElementBuilder":
		"""
		Shallow copy an element to create a new element object.
		"""
		element = ElementBuilder(context=self.context)
		element.attrs = self.attrs.copy()
		element.sequences = {kind: sequence for kind, sequence in self.sequences.items() if kind not in ignoreNested}
		return element

	def getIndexes(self, attr: typing.Optional[str] = None) -> typing.Tuple[int, int]:
		"""
		Calculate the index of this element.
		"""
		if attr is not None and self.isAttr(attr):
			return self.getAttr(attr).index, self.getAttr(attr).end

		# Use the begining and the end of the element.
		start = sys.maxsize
		end = 0
		for key, attrObj in self.getAttrs().items():
			# Ignore some attributes
			if attrObj.index == IGNORE_INDEX_VALUE:
				continue
			start = min(start, attrObj.index)
			end = max(end, attrObj.end)
		if start < sys.maxsize:
			return start, end

		# Invalid span.
		return IGNORE_INDEX_VALUE, 0

	def makeContext(self, attr: typing.Optional[str] = None) -> typing.Tuple[Context, int, int]:
		return self.context.resolve(element=self, attr=attr)

	def __eq__(self, other: object) -> bool:
		if not isinstance(other, Element):
			return False
		return isEqual(self.serialize(ignoreContext=False), other.serialize(ignoreContext=False))

	def __ne__(self, other: object) -> bool:
		return not (self == other)

	def toString(self, nested: bool = True) -> str:
		"""
		Human readable string representation of the element.
		"""

		contentContext = ["context=\"{}\"".format(self.context)] if self.context is not None else []
		content = "<Element {}/>".format(" ".join(
			["{}:{}:{}=\"{}\"".format(key, attr.index, attr.end, attr.value)
			for key, attr in self.attrs.items()] + contentContext))

		if nested:
			for kind, sequence in self.sequences.items():
				content += "\n{}:\n{}".format(kind, repr(sequence))

		return content

	def __repr__(self) -> str:
		return self.toString()


U = typing.TypeVar("U", bound="ElementBuilder")


class ElementBuilder(Element):

	@staticmethod
	def cast(element: Element, classType: typing.Type[U]) -> U:
		"""
		Return a copy of the element casted to a given type.
		"""
		assert isinstance(element, Element)
		assert isinstance(classType, type)
		assert issubclass(classType, ElementBuilder)
		copiedElement = copy.copy(element)
		copiedElement.__class__ = classType
		return typing.cast(U, copiedElement)

	def setAttr(self: U, key: str, value: str, index: int = IGNORE_INDEX_VALUE, end: int = 0) -> U:
		"""
		Add an attribute to the element.
		"""
		self.attrs[key] = AttributeParser(index=index, end=end, value=value)
		return self

	def setAttrs(self: U, data: typing.Dict[str, str]) -> U:
		"""
		Add multiple attributes to an element.
		"""
		for key, value in data.items():
			self.setAttr(key, value)
		return self

	def updateAttr(self: U, key: str, value: str) -> U:
		"""
		Update the value of an exsiting attribute.
		"""
		self.attrs[key].setValue(value)
		return self

	def setNestedSequence(self: U, kind: str, sequence: Sequence) -> U:
		"""
		Set a nested sequence and overwrite exsiting one.
		"""
		sequence.context.setParent(self)
		self.sequences[kind] = sequence
		return self

	def pushBackElementToNestedSequence(self: U, kind: str, element: Element) -> U:
		"""
		Add an element to a new or existing nested sequence.
		"""
		if kind not in self.sequences:
			self.sequences[kind] = Sequence(context=Context(parent=self))
		SequenceBuilder.cast(self.sequences[kind], SequenceBuilder).pushBackElement(element)
		return self

	def pushFrontElementToNestedSequence(self: U, kind: str, element: Element) -> U:
		"""
		Add an element to a new or existing nested sequence.
		"""
		if kind not in self.sequences:
			self.sequences[kind] = Sequence(context=Context(parent=self))
		SequenceBuilder.cast(self.sequences[kind], SequenceBuilder).pushFrontElement(element)
		return self

	def removeElementFromNestedSequence(self: U, kind: str, element: Element) -> U:
		"""
		Remove an element from a nested sequence.
		"""
		if kind in self.sequences:
			SequenceBuilder.cast(self.sequences[kind], SequenceBuilder).removeElement(element)
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
			self.sequences[kind] = SequenceParser(context=Context(parent=self), grammar=grammar, parent=self)
		return typing.cast(SequenceParser, self.sequences[kind]).makeElement()

	def getSequence(self) -> SequenceParser:
		assert not self.isRoot, "reached root element"
		assert isinstance(self.parent,
			SequenceParser), "parent must be a sequence, instead {}".format(type(self.parent))
		return self.parent
