import copy
import sys
import typing

from bzd.parser.context import Context
from bzd.parser.fragments import (
	IGNORE_INDEX_VALUE,
	Attribute,
	AttributeParser,
	Attributes,
	AttributesSerialize,
	Fragment,
)
from bzd.parser.grammar import Grammar
from bzd.utils.dict import isEqual

ElementSerialize = typing.MutableMapping[str, AttributesSerialize | list[typing.Any]]
SequenceSerialize = list[ElementSerialize]


class Sequence:
	def __init__(self, context: Context | None = None) -> None:
		self.context = Context() if context is None else context
		self.list: list[Element] = []

	def __iter__(self) -> typing.Iterator["Element"]:
		for element in self.list:
			if not element.isEmpty():
				yield element

	def __getitem__(self, index: int) -> "Element":
		return [*self][index]  # type: ignore

	def __len__(self) -> int:
		return len([e for e in self])

	def empty(self) -> bool:
		return len(self) == 0

	def __add__(self, other: typing.Union[list["Element"], "Sequence"]) -> "Sequence":
		if isinstance(other, Sequence):
			self.list += other.list
		else:
			self.list += other
		return self

	def __repr__(self) -> str:
		listStr = []
		for element in self.list:
			formattedStr = "- {}".format("\n  ".join(repr(element).split("\n")))
			listStr.append(formattedStr)
		return "\n".join(listStr)

	def copy(self) -> "SequenceBuilder":
		"""Create a copy of this sequence."""

		sequence = SequenceBuilder(context=self.context)
		sequence.list = self.list.copy()
		return sequence

	def deepCopy(self) -> "Sequence":
		"""Create a deep copy of this sequence."""

		serialized = self.serialize(ignoreContext=True)
		return Sequence.fromSerialize(serialized, context=self.context)

	def serialize(self, ignoreContext: bool = False) -> SequenceSerialize:
		"""Serialize a sequence."""

		return [element.serialize(ignoreContext=ignoreContext) for element in self]

	@staticmethod
	def fromSerialize(sequence: SequenceSerialize, context: Context | None = None) -> "Sequence":
		"""Create a sequence from a serialized sequence."""

		s = SequenceBuilder(context)
		for e in sequence:
			s.pushBackElement(Element.fromSerialize(e))
		return s


T = typing.TypeVar("T", bound="SequenceBuilder")


class SequenceBuilder(Sequence):
	"""Represents a sequence that can be built."""

	@staticmethod
	def cast(sequence: Sequence, classType: type[T]) -> T:
		"""Return a copy of the sequence casted to a given type."""

		assert isinstance(sequence, Sequence)
		assert isinstance(classType, type)
		assert issubclass(classType, SequenceBuilder)
		copiedSequence = copy.copy(sequence)
		copiedSequence.__class__ = classType
		return typing.cast(T, copiedSequence)

	def merge(self, sequence: "Sequence") -> typing.Self:
		for element in sequence.list:
			self.pushBackElement(element)
		return self

	def pushBackElement(self, element: "Element") -> typing.Self:
		"""Add an element to the sequence at the end of the list."""

		element.context.setParent(self)
		self.list.append(element)
		return self

	def pushFrontElement(self, element: "Element") -> typing.Self:
		"""Add an element to the sequence at the beginning of the list."""

		element.context.setParent(self)
		self.list.insert(0, element)
		return self

	def removeElement(self, element: "Element") -> typing.Self:
		"""Remove an element from the sequence."""

		element.context.clear()
		self.list.remove(element)
		return self


class SequenceParser(Sequence):
	"""This represents a sequence of Elements."""

	def __init__(
		self,
		context: Context | None,
		grammar: Grammar,
		parent: typing.Optional["ElementParser"],
	) -> None:
		super().__init__(context)
		self.grammar = grammar
		self.parent = parent

	def makeElement(self, grammar: Grammar | None = None) -> "ElementParser":
		"""Create a new element in the sequence.
		Params:
		- grammar: Optionally provides a grammar to the new element or reuse existing one.
		"""
		element = ElementParser(
			context=Context(parent=self),
			grammar=self.grammar if grammar is None else grammar,
			parent=self,
		)
		self.list.append(element)
		return element

	def getGrammar(self) -> Grammar:
		return self.grammar

	def getElement(self) -> "ElementParser":
		assert self.parent, "This sequence is at the top level."
		return self.parent


class Element:
	def __init__(self, context: Context | None = None) -> None:
		self.context = Context() if context is None else context
		self.attrs: Attributes = {}
		self.sequences: dict[str, Sequence] = {}

	@staticmethod
	def fromSerialize(element: ElementSerialize, context: Context | None = None) -> "Element":
		"""Create an element from a serialized element."""

		e = Element(context)
		assert isinstance(element["@"], dict)
		e.attrs = {key: Attribute.fromSerialize(attr) for key, attr in element["@"].items()}
		e.sequences = {
			key: Sequence.fromSerialize(sequence, Context(parent=e))  # type: ignore
			for key, sequence in element.items()
			if key != "@"
		}
		return e

	def isEmpty(self) -> bool:
		"""Check whether or not an element is empty. Empty means with no data."""

		return (len(self.attrs.keys()) + len(self.sequences.keys())) == 0

	def getAttrs(self) -> typing.Mapping[str, Attribute]:
		"""Return the attributes as a dictionary."""

		return self.attrs

	def getAttr(self, name: str) -> Attribute:
		"""Return a specific name attribute value."""

		assert name in self.attrs, f"Attribute '{name}' is not present in '{self!s}'."
		return self.attrs[name]

	def getAttrValue(self, name: str, default: str | None = None) -> str | None:
		"""Return a specific name attribute value."""

		if name in self.attrs:
			return self.attrs[name].value
		return default

	def isAttr(self, name: str) -> bool:
		"""Check if an attribute is present."""

		return name in self.attrs

	def isNestedSequence(self, kind: str) -> bool:
		"""Check if a sequence exists."""

		return bool(kind in self.sequences)

	def getNestedSequence(self, kind: str) -> Sequence | None:
		"""Get the current sequence, or None if it does not exist."""

		assert kind != "@", "Nested sequence name '@' cannot be used."
		return self.sequences.get(kind, None)

	def getNestedSequenceAssert(self, kind: str) -> Sequence:
		"""Get the current sequence, or assert if it does not exist."""

		maybeSequence = self.getNestedSequence(kind)
		assert maybeSequence is not None, "Sequence must be non-null."
		return maybeSequence

	def getNestedSequenceOrEmpty(self, kind: str) -> Sequence:
		"""Get the current sequence, or an empty one if it does not exist."""

		maybeSequence = self.getNestedSequence(kind)
		if maybeSequence is None:
			return Sequence()
		return maybeSequence

	def getNestedSequences(self) -> typing.Iterator[tuple[str, Sequence]]:
		"""Generator going through all sequences."""

		yield from self.sequences.items()

	def serialize(self, ignoreContext: bool = False) -> ElementSerialize:
		"""Serialize an element."""

		data: ElementSerialize = {
			"@": {key: attr.serialize(ignoreContext=ignoreContext) for key, attr in self.getAttrs().items()}
		}
		for kind, sequence in self.getNestedSequences():
			data[kind] = sequence.serialize(ignoreContext=ignoreContext)
		return data

	def copy(self, ignoreNested: list[str] | None = None) -> "ElementBuilder":
		"""Shallow copy an element to create a new element object."""

		if ignoreNested is None:
			ignoreNested = []
		element = ElementBuilder(context=self.context)
		element.attrs = copy.deepcopy(self.attrs)
		element.sequences = {kind: sequence for kind, sequence in self.sequences.items() if kind not in ignoreNested}
		return element

	def deepCopy(self, ignoreNested: list[str] | None = None) -> "ElementBuilder":
		"""Deep copy an element to create a new element object."""

		if ignoreNested is None:
			ignoreNested = []
		element = ElementBuilder(context=self.context)
		element.attrs = copy.deepcopy(self.attrs)
		element.sequences = {
			kind: sequence.deepCopy() for kind, sequence in self.sequences.items() if kind not in ignoreNested
		}
		return element

	def getIndexes(self, attr: str | None = None) -> tuple[int, int]:
		"""Calculate the index of this element."""

		if attr is not None and self.isAttr(attr):
			return self.getAttr(attr).index, self.getAttr(attr).end

		# Use the beginning and the end of the element.
		start = sys.maxsize
		end = 0
		for attrObj in self.getAttrs().values():
			# Ignore some attributes
			if attrObj.index == IGNORE_INDEX_VALUE:
				continue
			start = min(start, attrObj.index)
			end = max(end, attrObj.end)
		if start < sys.maxsize:
			return start, end

		# Invalid span.
		return IGNORE_INDEX_VALUE, 0

	def makeContext(self, attr: str | None = None) -> tuple[Context, int, int]:
		return self.context.resolve(element=self, attr=attr)

	def __eq__(self, other: object) -> bool:
		if not isinstance(other, Element):
			return False
		return isEqual(self.serialize(ignoreContext=False), other.serialize(ignoreContext=False))

	def __ne__(self, other: object) -> bool:
		return not (self == other)

	def toString(self, nested: bool = True, simplified: bool = False) -> str:
		"""Human readable string representation of the element."""

		contentContext = [] if simplified else ([f'context="{self.context}"'] if self.context is not None else [])
		attributeFormat = '{key}="{value}"' if simplified else '{key}:{index}:{end}="{value}"'
		content = "<Element {}/>".format(
			" ".join(
				[
					attributeFormat.format(key=key, index=attr.index, end=attr.end, value=attr.value)
					for key, attr in self.attrs.items()
					if attr.value is not None
				]
				+ contentContext
			)
		)

		if nested:
			for kind, sequence in self.sequences.items():
				content += f"\n{kind}:\n{sequence!r}"

		return content

	def __repr__(self) -> str:
		return self.toString()


U = typing.TypeVar("U", bound="ElementBuilder")


class ElementBuilder(Element):
	@staticmethod
	def cast(element: Element, classType: type[U]) -> U:
		"""Return a copy of the element casted to a given type."""

		assert isinstance(element, Element)
		assert isinstance(classType, type)
		assert issubclass(classType, ElementBuilder)
		copiedElement = copy.copy(element)
		copiedElement.__class__ = classType
		return typing.cast(U, copiedElement)

	def get(self) -> Element:
		"""Return the underlying element class."""

		self.__class__ = Element  # type: ignore
		return typing.cast(Element, self)

	def removeAttr(self, key: str) -> typing.Self:
		"""Remove an existing attribute from the element."""

		del self.attrs[key]
		return self

	def setAttr(self, key: str, value: str, index: int = IGNORE_INDEX_VALUE, end: int = 0) -> typing.Self:
		"""Add an attribute to the element."""

		self.attrs[key] = AttributeParser(index=index, end=end, value=value)
		return self

	def setAttrs(self, data: dict[str, str]) -> typing.Self:
		"""Add multiple attributes to an element."""

		for key, value in data.items():
			self.setAttr(key, value)
		return self

	def updateAttr(self, key: str, value: str) -> typing.Self:
		"""Update the value of an existing attribute."""

		self.attrs[key].setValue(value)
		return self

	def setNestedSequence(self, kind: str, sequence: Sequence) -> typing.Self:
		"""Set a nested sequence and overwrite existing one."""

		sequence.context.setParent(self)
		self.sequences[kind] = sequence
		return self

	def removeNestedSequence(self, kind: str) -> typing.Self:
		"""Remove an existing nested sequence."""

		del self.sequences[kind]
		return self

	def pushBackElementToNestedSequence(self, kind: str, element: Element) -> typing.Self:
		"""Add an element to a new or existing nested sequence."""

		if kind not in self.sequences:
			self.sequences[kind] = Sequence(context=Context(parent=self))
		SequenceBuilder.cast(self.sequences[kind], SequenceBuilder).pushBackElement(element)
		return self

	def pushFrontElementToNestedSequence(self, kind: str, element: Element) -> typing.Self:
		"""Add an element to a new or existing nested sequence."""

		if kind not in self.sequences:
			self.sequences[kind] = Sequence(context=Context(parent=self))
		SequenceBuilder.cast(self.sequences[kind], SequenceBuilder).pushFrontElement(element)
		return self

	def removeElementFromNestedSequence(self, kind: str, element: Element) -> typing.Self:
		"""Remove an element from a nested sequence."""

		if kind in self.sequences:
			SequenceBuilder.cast(self.sequences[kind], SequenceBuilder).removeElement(element)
		return self


class ElementParser(Element):
	def __init__(
		self,
		context: Context | None,
		grammar: Grammar,
		parent: SequenceParser | None = None,
	) -> None:
		super().__init__(context)
		self.grammar = grammar
		self.parent = parent

	@property
	def isRoot(self) -> bool:
		return self.parent is None

	def add(self, fragment: Fragment) -> None:
		"""Add a new fragment to this element."""

		fragment.merge(self.attrs)

	def setGrammar(self, grammar: Grammar) -> None:
		"""Update the grammar of the current element."""

		self.grammar = grammar

	def getGrammar(self) -> Grammar:
		return self.grammar

	def makeElement(self, kind: str, grammar: Grammar) -> "ElementParser":
		if kind not in self.sequences:
			self.sequences[kind] = SequenceParser(
				context=Context(parent=self),
				grammar=grammar,
				parent=self,
			)
		return typing.cast(SequenceParser, self.sequences[kind]).makeElement()

	def getSequence(self) -> SequenceParser:
		assert not self.isRoot, "reached root element"
		assert isinstance(self.parent, SequenceParser), f"parent must be a sequence, instead {type(self.parent)}"
		return self.parent
