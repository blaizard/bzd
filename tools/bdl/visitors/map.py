"""
Create an object map.
"""
import typing

from bzd.parser.error import Error

from tools.bdl.visitor import Visitor
from tools.bdl.result import SymbolType
from tools.bdl.entity.variable import Variable
from tools.bdl.entity.nested import Nested
from tools.bdl.entity.method import Method
from tools.bdl.entity.using import Using
from tools.bdl.entity.enum import Enum

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

		# Save the serialized payload
		payload = entity.element.serialize(ignoreNested=["nested"])
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
