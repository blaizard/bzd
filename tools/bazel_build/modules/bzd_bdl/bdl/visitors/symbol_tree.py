from bdl.entities.all import EntityType
from bdl.entities.impl.fragment.sequence import EntitySequence
from bdl.visitors.symbol_map import SymbolMap


class SymbolTree(EntitySequence):
	def __init__(self, symbols: SymbolMap) -> None:
		self.symbols = symbols
		self.fqns_: list[str] = []
		super().__init__([])

	@property
	def sequence(self) -> list[EntityType]:
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
		assert self.symbols.contains(entity.fqn), f"This FQN '{entity.fqn}' is not registered in the symbol map."
		self.sequence_.append(entity)

	def entity(self, fqn: str) -> EntityType:
		"""Get the entity referred to the given fqn."""
		return self.symbols.getEntityResolved(fqn=fqn).value

	def serialize(self) -> list[str]:
		"""
		Return a serialized version of this tree.
		"""
		return [entity.fqn for entity in self.sequence]

	@staticmethod
	def fromSerialize(data: list[str], symbols: SymbolMap) -> "SymbolTree":
		"""
		Create a symbol tree from a serialized object.
		"""
		tree = SymbolTree(symbols=symbols)
		# Checks
		for fqn in data:
			assert symbols.contains(fqn=fqn), f"FQN '{fqn}' not found in the symbol map."
		# Lazy conversion from FQN to symbols
		tree.fqns_ = data
		return tree
