import typing

from bzd.parser.element import ElementBuilder as Element, SequenceBuilder as Sequence
from bzd.validation.validation import Validation
from tools.bdl.contracts.contract import Contract


class ElementBuilder(Element):

	def __init__(self, category: str) -> None:
		super().__init__()
		self.addAttr("category", category)

	def addContract(self, contract: str) -> "ElementBuilder":
		"""
		Add a contract to the element.
		"""
		parsed = Validation.parse(contract)
		for kind, values in parsed.items():
			Contract.add(element=self, kind=kind, values=values)
		return self

	def addConfig(
			self,
			kind: str,
			name: typing.Optional[str] = None,
			contract: typing.Optional[str] = None) -> "ElementBuilder":
		"""
		Create a configuration entry
		"""
		element = ElementBuilder(category="expression").addAttr("type", kind)
		if name is not None:
			element.addAttr("name", name)
		if contract is not None:
			element.addContract(contract)
		self.pushBackElementToNestedSequence(kind="config", element=element)
		return self


class SequenceBuilder(Sequence):
	pass
