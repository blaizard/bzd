import typing

from bzd.parser.element import Element, ElementBuilder
from bzd.parser.error import Error


class Contract:

	def __init__(self, element: Element) -> None:

		Error.assertHasAttr(element=element, attr="type")
		self.element = element

	@staticmethod
	def add(element: Element, kind: str, values: typing.Optional[typing.List[str]] = None) -> None:
		"""
		Add a contract to the element.
		"""
		contractElement = ElementBuilder().addAttr("type", kind)
		if values is not None:
			for value in values:
				valueElement = ElementBuilder().addAttr("value", value)
				contractElement.pushBackElementToNestedSequence(kind="values", element=valueElement)
		ElementBuilder.cast(element, ElementBuilder).pushBackElementToNestedSequence(kind="contract",
			element=contractElement)

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

	def __repr__(self) -> str:
		if self.isValue:
			return "{}({})".format(self.type, ", ".join(self.values))
		return self.type
