import typing

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.visitor import Visitor
from bzd.parser.error import Error

from bzd.validation.validation import Validation

_CONTRACT_VALUE_TYPES = {"min", "max", "integer", "float", "string", "boolean"}


class Contract:

	def __init__(self, element: Element) -> None:

		Error.assertHasAttr(element=element, attr="type")
		self.element = element

	@property
	def type(self) -> str:
		return self.element.getAttr("type").value

	@property
	def values(self) -> typing.List[str]:
		values = self.element.getNestedSequence("values")
		if values:
			return [e.getAttr("value").value for e in values]
		return []

	@property
	def value(self) -> typing.Optional[str]:
		values = self.element.getNestedSequence("values")
		if values:
			return values[0].getAttr("value").value
		return None

	@property
	def valueNumber(self) -> float:
		try:
			return float(self.valueString)
		except:
			Error.handleFromElement(element=self.element, message="Expected a valid number.")
		return 0.0  # To make mypy happy

	@property
	def valueString(self) -> str:
		value = self.value
		if value is None:
			Error.handleFromElement(element=self.element, message="A value must be present.")
		assert value is not None
		return value

	@property
	def isValue(self) -> bool:
		return self.element.isNestedSequence("values")

	@property
	def comment(self) -> typing.Optional[str]:
		return self.element.getAttrValue("comment")


class Contracts:
	"""
	A contract is an unsorted sequence of types associated with an optional value and/or commment.
	"""

	def __init__(self, element: Element, sequenceKind: str = "contract") -> None:
		self.element = element
		self.sequenceKind = sequenceKind

	def get(self, kind: str) -> typing.Optional[Contract]:
		"""
		Get a specific contract type if present.
		"""
		for contract in self:
			if contract.type == kind:
				return contract
		return None

	@property
	def validationValue(self) -> typing.Optional[str]:
		"""
		Generate a validation object for a value out of the current contracts
		"""
		content = []
		for contract in self:
			if contract.type in _CONTRACT_VALUE_TYPES:
				content.append("{}({})".format(contract.type, ",".join(contract.values)))
		return " ".join(content) if content else None

	@property
	def empty(self) -> bool:
		sequence = self.element.getNestedSequence(self.sequenceKind)
		if sequence is None:
			return True
		return (len(sequence) == 0)

	def mergeBase(self, contracts: "Contracts") -> None:
		"""
		Merge a base contract into this one. The order is important, as type are inherited from the deepest base.
		"""

		elementBuilder = ElementBuilder.cast(self.element, ElementBuilder)
		for contract in reversed([*contracts]):
			elementBuilder.pushFrontElementToNestedSequence(self.sequenceKind, contract.element)

	def __iter__(self) -> typing.Iterator[Contract]:
		sequence = self.element.getNestedSequence(self.sequenceKind)
		if sequence:
			for element in sequence:
				yield Contract(element)

	def __repr__(self) -> str:
		content = []
		for contract in self:
			content.append("{}({})".format(contract.type, ",".join(contract.values)))
		return " ".join(content)
