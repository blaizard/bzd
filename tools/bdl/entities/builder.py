import typing

from bzd.parser.element import ElementBuilder as Element, SequenceBuilder as Sequence
from bzd.validation.validation import Validation
from tools.bdl.contracts.contract import Contract


class ElementBuilder(Element):

	def __init__(self, category: str) -> None:
		super().__init__()
		self.setAttr("category", category)

	@staticmethod
	def makeFrom(element: Element) -> "ElementBuilder":
		"""
		Copy an element from an existing one.
		"""
		return ElementBuilder.cast(element, ElementBuilder)

	def setConst(self) -> "ElementBuilder":
		self.setAttr("const", "")
		return self

	def removeConst(self) -> "ElementBuilder":
		if self.isAttr("const"):
			self.removeAttr("const")
		return self

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
		element = ElementBuilder(category="expression").setAttr("type", kind)
		if name is not None:
			element.setAttr("name", name)
		if contract is not None:
			element.addContract(contract)
		self.pushBackElementToNestedSequence(kind="config", element=element)
		return self


class NamespaceBuilder(ElementBuilder):

	def __init__(self, namespace: typing.List[str]) -> None:
		super().__init__("namespace")
		for name in namespace:
			element = Element().setAttr("name", name)
			self.pushBackElementToNestedSequence(kind="name", element=element)


class SequenceBuilder(Sequence):
	pass
