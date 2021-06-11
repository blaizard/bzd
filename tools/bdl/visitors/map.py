"""
Create an object map.
"""
import typing

from bzd.parser.error import Error
from bzd.parser.element import Sequence

from tools.bdl.visitor import Visitor
from tools.bdl.result import SymbolType
from tools.bdl.entities.all import Variable, Nested, Method, Using, Enum

MapType = typing.Dict[str, typing.Any]


class Map(Visitor[MapType]):

	def __init__(self) -> None:
		super().__init__()
		self.map: MapType = {}

	def visitFinal(self, result: MapType) -> MapType:
		return self.map

	def mapEntity(self, entity: SymbolType) -> None:

		# Build the symbol name and ensure it is unique
		symbol = self.makeFQN(entity.name)
		Error.assertTrue(element=entity.element,
			condition=(symbol not in self.map),
			message="Symbol name is in conflict with a previous one {}.".format(symbol))

		# Save the serialized payload and convert nested dependencies and make them links
		element = entity.element
		nested = element.getNestedSequence("nested")
		if nested:
			references = [
				self.makeEntity(category="reference", attrs={"symbol": element.getAttr("name")})
				for element in nested.iterate() if element.isAttr("name")
			]
			sequence = Sequence.fromSerialize(references)
			element = element.copy(ignoreNested=["nested"])
			element.setNestedSequence("nested", sequence)

		payload = element.serialize()
		self.map[symbol] = payload

	def visitNestedEntities(self, entity: Nested, result: MapType) -> None:
		self.mapEntity(entity=entity)

	def visitVariable(self, entity: Variable, result: MapType) -> None:
		self.mapEntity(entity=entity)

	def visitMethod(self, entity: Method, result: MapType) -> None:
		self.mapEntity(entity=entity)

	def visitUsing(self, entity: Using, result: MapType) -> None:
		self.mapEntity(entity=entity)

	def visitEnum(self, entity: Enum, result: MapType) -> None:
		self.mapEntity(entity=entity)
