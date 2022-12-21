import typing

from bzd.parser.element import ElementBuilder as Element, SequenceBuilder as Sequence
from bzd.validation.validation import Validation
from tools.bdl.contracts.contract import Contract


class ElementBuilder(Element):

	def __init__(self, category: str) -> None:
		super().__init__()
		self.setAttr("category", category)

	def addContract(self, contract: str) -> "ElementBuilder":
		"""
		Add a contract to the element.
		"""
		parsed = Validation.parse(contract)
		for kind, values in parsed.items():
			Contract.add(element=self, kind=kind, values=values)
		return self

	def addConfigValue(
			self,
			kind: typing.Optional[str] = None,
			literal: typing.Optional[str] = None,
			name: typing.Optional[str] = None,
			contract: typing.Optional[str] = None) -> "ElementBuilder":
		"""
		Create a configuration entry
		"""
		element = ElementBuilder(category="expression")
		if kind is not None:
			element.setAttr("type", kind)
		if literal is not None:
			element.setAttr("value", literal)
			element.setAttr("literal", literal)
		if name is not None:
			element.setAttr("name", name)
		if contract is not None:
			element.addContract(contract)
		self.pushBackElementToNestedSequence(kind="config", element=element)
		return self

	def addConfigType(
			self,
			kind: str,
			name: str,
			contract: typing.Optional[str] = None) -> "ElementBuilder":
		"""
		Create a configuration entry
		"""
		element = ElementBuilder(category="using").setAttr("type", kind).setAttr("name", name)
		if contract is not None:
			element.addContract(contract)
		self.pushBackElementToNestedSequence(kind="config", element=element)
		return self

class NestedBuilder(ElementBuilder):

	def __init__(self, kind: str) -> None:
		super().__init__(kind)


class NamespaceBuilder(ElementBuilder):

	def __init__(self, namespace: typing.List[str]) -> None:
		super().__init__("namespace")
		for name in namespace:
			element = Element().setAttr("name", name)
			self.pushBackElementToNestedSequence(kind="name", element=element)


class MethodBuilder(ElementBuilder):

	def __init__(self, name: str) -> None:
		super().__init__("method")
		self.setAttr("name", name)
		self.setAttr("fqn", name)


class ExpressionBuilder(ElementBuilder):

	def __init__(self, type: str, name: typing.Optional[str] = None) -> None:
		super().__init__("expression")
		if name is not None:
			self.setAttr("name", name)
		self.setAttr("type", type)


class SequenceBuilder(Sequence):
	pass
