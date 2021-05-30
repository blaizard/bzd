import typing

from bzd.parser.element import Element, Sequence
from bzd.parser.visitor import Visitor
from bzd.parser.error import handleFromElement, assertHasAttr


class _Contract(typing.NamedTuple):
	kind: str
	value: typing.Optional[str]
	comment: typing.Optional[str]


class _VisitorContract(Visitor[_Contract, typing.List[_Contract]]):

	nestedKind = None

	def __init__(self) -> None:
		self.uniqueTypes: typing.Set[str] = set()

	def visitBegin(self, result: typing.Any) -> typing.List[_Contract]:
		return []

	def visitElement(self, element: Element, result: typing.List[_Contract]) -> typing.List[_Contract]:

		assertHasAttr(element=element, attr="type")
		kind = element.getAttr("type").value
		if kind in self.uniqueTypes:
			handleFromElement(element=element, message="The contract type '{}' is set twice.".format(kind))
		self.uniqueTypes.add(kind)

		result.append(_Contract(kind=kind, value=element.getAttrValue("value"),
			comment=element.getAttrValue("comment")))

		return result


class Contracts:
	"""
	A contract is an unsorted sequence of kind associated with an optional value and/or commment.
	"""

	def __init__(self, sequence: Sequence) -> None:
		visitor = _VisitorContract()
		self.data = visitor.visit(sequence)

	def get(self, kind: str) -> typing.Optional[_Contract]:
		"""
		Get a specific contract type if present.
		"""
		for contract in self.data:
			if contract.kind == kind:
				return contract
		return None

	def __iter__(self) -> typing.Iterator[_Contract]:
		return self.data.__iter__()
