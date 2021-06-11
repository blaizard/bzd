import typing

from bzd.parser.element import Element


class Entity:

	def __init__(self, element: Element) -> None:
		self.element = element

	@property
	def name(self) -> str:
		return self.element.getAttr("name").value

	def toString(self, attrs: typing.MutableMapping[str, str] = {}) -> str:
		entities = ["{}=\"{}\"".format(key, value) for key, value in attrs.items()]
		return "<{}/>".format(" ".join([type(self).__name__] + entities))

	def __repr__(self) -> str:
		"""
		Human readable string representation of a result.
		"""
		return self.toString()


def EntityFactory(isSymbol: bool) -> typing.Type[Entity]:

	class Factory(Entity):

		@property
		def isSymbol(self) -> bool:
			return isSymbol

	return Factory
