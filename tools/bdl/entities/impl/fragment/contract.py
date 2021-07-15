import typing

from bzd.parser.element import Element
from bzd.parser.visitor import Visitor
from bzd.parser.error import Error

from bzd.validation.validation import Validation


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
		value = self.element.getAttrValue("value")
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


class _VisitorContract(Visitor[Contract, typing.List[Contract]]):

	nestedKind = None

	def __init__(self) -> None:
		self.uniqueTypes: typing.Set[str] = set()

	def visitBegin(self, result: typing.Any) -> typing.List[Contract]:
		return []

	def visitElement(self, element: Element, result: typing.List[Contract]) -> typing.List[Contract]:

		contract = Contract(element=element)

		if contract.type in self.uniqueTypes:
			Error.handleFromElement(element=element,
				message="The contract type '{}' is set twice.".format(contract.type))
		self.uniqueTypes.add(contract.type)

		result.append(contract)

		return result


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
	def validationValue(self) -> str:
		"""
		Generate a validation object for a value out of the current contracts
		"""
		content = []
		maybeMin = self.get("min")
		if maybeMin:
			content.append("min({})".format(maybeMin.valueString))
		maybeMax = self.get("max")
		if maybeMax:
			content.append("max({})".format(maybeMax.valueString))
		return " ".join(content)

	@property
	def empty(self) -> bool:
		sequence = self.element.getNestedSequence(self.sequenceKind)
		if sequence is None:
			return True
		return (len(sequence) == 0)

	def merge(self, contracts: "Contracts") -> None:
		"""
		Merge a new contract with this one.
		"""
		sequence = self.element.getNestedSequence(self.sequenceKind)
		#if sequence is None:
		#	contracts.element.getNestedSequence(self.sequenceKind)
		#self.sequence.merge(contracts.sequence)
		pass

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
