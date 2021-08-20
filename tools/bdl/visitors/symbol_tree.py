import typing

from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.entities.all import EntityType
from tools.bdl.entities.impl.fragment.sequence import EntitySequence


class SymbolTree(EntitySequence):

	def __init__(self, symbols: SymbolMap) -> None:
		self.symbols = symbols
		self.fqns_: typing.List[str] = []
		super().__init__([])

	@property
	def sequence(self) -> typing.List[EntityType]:
		"""
		Used to enable lazy loading of entities. Only when used they will be resolved.
		This should speed-up the bdl object merge as the symbol tree is not touch during this process.
		"""
		if len(self.fqns_) > len(self.sequence_):
			self.sequence_ = []
			for fqn in self.fqns_:
				self.addEntity(entity=self.symbols.getEntityResolved(fqn=fqn).value)
		return self.sequence_

	def addEntity(self, entity: EntityType) -> None:
		entity.assertTrue(condition=entity.isFQN, message="Entity is missing FQN attribute.")
		assert self.symbols.contains(entity.fqn), "This FQN '{}' is not registered in the symbol map.".format(
			entity.fqn)
		self.sequence_.append(entity)

	def serialize(self) -> typing.List[str]:
		"""
		Return a serialized version of this tree.
		"""
		return [entity.fqn for entity in self.sequence]

	@staticmethod
	def fromSerialize(data: typing.List[str], symbols: SymbolMap) -> "SymbolTree":
		"""
		Create a symbol map from a serialized object.
		"""
		tree = SymbolTree(symbols=symbols)
		# Checks
		for fqn in data:
			assert symbols.contains(fqn=fqn), "FQN '{}' not found in the symbol map.".format(fqn)
		# Lazy conversion from FQN to symbols
		tree.fqns_ = data
		return tree
