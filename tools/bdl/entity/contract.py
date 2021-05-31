import typing

from bzd.parser.element import Element, Sequence
from bzd.parser.visitor import Visitor
from bzd.parser.error import handleFromElement, assertHasAttr


class Contract:

	def __init__(self, element: Element, visitor: typing.Any) -> None:

		assertHasAttr(element=element, attr="type")

		self.element = element
		self.visitor = visitor

	@property
	def type(self) -> str:
		return self.element.getAttr("type").value

	@property
	def value(self) -> typing.Optional[str]:
		return self.element.getAttrValue("value")

	@property
	def isValue(self) -> bool:
		return self.element.isAttr("value")

	@property
	def comment(self) -> typing.Optional[str]:
		return self.visitor.visitComment("contract", self.element.getAttrValue("comment"))


class _VisitorContract(Visitor[Contract, typing.List[Contract]]):

	nestedKind = None

	def __init__(self, visitor: typing.Any) -> None:
		self.uniqueTypes: typing.Set[str] = set()
		self.visitor = visitor

	def visitBegin(self, result: typing.Any) -> typing.List[Contract]:
		return []

	def visitElement(self, element: Element, result: typing.List[Contract]) -> typing.List[Contract]:

		contract = Contract(element=element, visitor=self.visitor)

		if contract.type in self.uniqueTypes:
			handleFromElement(element=element, message="The contract type '{}' is set twice.".format(contract.type))
		self.uniqueTypes.add(contract.type)

		result.append(contract)

		return result


class Contracts:
	"""
	A contract is an unsorted sequence of types associated with an optional value and/or commment.
	"""

	def __init__(self, sequence: typing.Optional[Sequence], visitor: typing.Any) -> None:
		self.data = _VisitorContract(visitor=visitor).visit(sequence) if sequence else []

	def get(self, type: str) -> typing.Optional[Contract]:
		"""
		Get a specific contract type if present.
		"""
		for contract in self.data:
			if contract.type == type:
				return contract
		return None

	@property
	def empty(self) -> bool:
		return not bool(self.data)

	def __iter__(self) -> typing.Iterator[Contract]:
		return self.data.__iter__()
