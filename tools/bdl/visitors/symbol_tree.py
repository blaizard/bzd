import typing

from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.entities.all import EntityType
from tools.bdl.entities.impl.fragment.sequence import EntitySequence


class SymbolTree(EntitySequence):

	def __init__(self, symbols: SymbolMap) -> None:
		self.symbols = symbols
		self.entites: typing.List[EntityType] = []
		super().__init__(self.entites)

	def addEntity(self, entity: EntityType) -> None:
		entity.assertTrue(condition=entity.isFQN, message="Entity is missing FQN attribute.")
		assert self.symbols.contains(entity.fqn), "This FQN '{}' is not registered in the symbol map.".format(
			entity.fqn)
		self.entites.append(entity)

	def serialize(self) -> typing.List[str]:
		"""
		Return a serialized version of this tree.
		"""
		return [entity.fqn for entity in self.entites]

	@staticmethod
	def fromSerialize(data: typing.List[str], symbols: SymbolMap) -> "SymbolTree":
		"""
		Create a symbol map from a serialized object.
		"""
		tree = SymbolTree(symbols=symbols)
		for fqn in data:
			result = symbols.getEntityResolved(fqn=fqn)
			assert result, "FQN '{}' not found in the symbol map.".format(fqn)
			tree.addEntity(entity=result.value)
		return tree
