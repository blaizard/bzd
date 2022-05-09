import typing
import dataclasses

from tools.bdl.visitor import CATEGORY_COMPOSITION
from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.entities.impl.builtin import Builtin
from tools.bdl.entities.impl.entity import Entity
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.builder import ExpressionBuilder


class DependencyGroup:
	"""List of ordered dependencies."""

	def __init__(self) -> None:
		self.data: typing.List[Entity] = []

	def copy(self) -> "DependencyGroup":
		"""Create a copy of this object."""
		dependencies = DependencyGroup()
		dependencies.data = self.data.copy()
		return dependencies

	def push(self, entity: Entity) -> None:
		"""Add a new dependency to the list."""
		# Ignore Builtin dependencies, as they are expected to be available at all time.
		if isinstance(entity, Builtin):
			return
		if entity in self.data:
			return
		self.data.append(entity)

	def pushGroup(self, group: typing.Union["DependencyGroup", typing.List[Entity]]) -> None:
		"""Push an ordered group of dependency to the list."""
		for entity in group:
			self.push(entity)

	def __add__(self, other: "DependencyGroup") -> "DependencyGroup":
		dependencies = self.copy()
		dependencies.pushGroup(other)
		return dependencies

	def __iter__(self) -> typing.Iterator[Entity]:
		for entity in self.data:
			yield entity

	def __len__(self) -> int:
		return len(self.data)

	def __repr__(self) -> str:
		return ", ".join([str(e) for e in self.data])


@dataclasses.dataclass
class Dependencies:
	executor: typing.Optional[Entity] = None
	# Dependencies over other entities
	deps: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	# Direct dependencies
	pre: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	intra: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	post: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)

	def __repr__(self) -> str:
		content = []
		content += [f"executor: {str(self.executor)}"]
		content += [f"deps: {self.deps}"]
		content += [f"pre: {self.pre}"]
		content += [f"intra: {self.intra}"]
		content += [f"post: {self.post}"]
		return "\n".join(content)


class DependencyMap:
	"""Class to handle entities and their dependencies."""

	def __init__(self, symbols: SymbolMap) -> None:
		self.symbols = symbols
		self.map: typing.Dict[Entity, Dependencies] = {}

	def findAllIntra(self, entities: typing.List[Entity]) -> DependencyGroup:
		"""Find all intra expressions associated with this entities."""

		dependencies = DependencyGroup()
		for entity in entities:
			assert entity in self.map, f"The entity {str(entity)} is not registered in the map."
			deps = self.findAllIntra([*self.map[entity].deps])
			dependencies.pushGroup(deps)
			dependencies.pushGroup([*self.map[entity].intra])

		return dependencies

	def add(self, entity: Entity) -> None:
		assert entity not in self.map, f"The entity '{entity}' is already inserted in the dependency map."
		dependencies = self.resolveDependencies(entity)

		#print("----------------", entity.fqn)
		#print(f"component: {str(entity)}")
		#print(str(dependencies))

		self.map[entity] = dependencies

	def addImplicit(self) -> None:
		"""Look for all implicit dependencies and add them if not already present."""

		while True:
			updated = {}
			for entity, dependencies in self.map.items():
				for dependency in dependencies.deps + dependencies.intra:
					if dependency not in self.map and dependency not in updated:
						updated[dependency] = self.resolveDependencies(dependency)
			if len(updated) == 0:
				break
			self.map.update(updated)

	def resolveDependencies(self, entity: Entity) -> Dependencies:
		"""Resolve the dependencies for a specific entity."""

		def checkIfInitOrShutdown(interfaceEntity: Entity) -> typing.Optional[DependencyGroup]:
			if interfaceEntity.contracts.has("init"):
				return dependencies.pre
			if interfaceEntity.contracts.has("shutdown"):
				return dependencies.post
			return None

		# These corresponds to the entities to be executed before / during / after.
		dependencies = Dependencies()

		# Resolve the entity against its namespace.
		resolver = self.symbols.makeResolver(namespace=entity.namespace)
		entity.resolveMemoized(resolver=resolver)

		if isinstance(entity, Expression):

			if entity.executor:
				dependencies.executor = self.symbols.getEntityResolved(fqn=entity.executor).assertValue(
					element=entity.element)

			# Look for dependencies coming from parameters if any.
			if entity.parameters:
				dependencies.deps.pushGroup(
					[self.symbols.getEntityResolved(fqn=fqn).value for fqn in entity.parameters.dependencies])

		entityUnderlyingType = entity.getEntityUnderlyingTypeResolved(resolver=resolver)

		# Check if there are pre/post-requisites (init/shutdown functions)
		if entityUnderlyingType.isInterface:
			for interfaceEntity in entityUnderlyingType.interface:
				maybeGroup = checkIfInitOrShutdown(interfaceEntity)
				if maybeGroup is not None:

					# Insert a new entry in the symbol map and resolve it against this entity.
					fqn = self.symbols.insert(name=None,
						namespace=interfaceEntity.namespace,
						path=None,
						element=ExpressionBuilder(type=f"this.{interfaceEntity.name}"),
						category=CATEGORY_COMPOSITION)
					newEntity = self.symbols.getEntityResolved(fqn=fqn).value
					newResolver = self.symbols.makeResolver(namespace=interfaceEntity.namespace, this=entity.fqn)
					newEntity.resolve(resolver=newResolver)
					maybeGroup.push(newEntity)

		# Check if there are dependent composition from this entity.
		if entityUnderlyingType.isComposition:
			for compositionEntity in entityUnderlyingType.composition:
				# Composition must only contain unamed entries.
				compositionEntity.assertTrue(condition=not compositionEntity.isName,
					message="Variable cannot be created within a nested composition.")
				assert isinstance(compositionEntity, Expression), "All composition entities must be an expression."

				# Create a new entity and associate it with its respective objects.
				entityCopied = compositionEntity.copy()
				newResolver = self.symbols.makeResolver(namespace=entityCopied.namespace, this=entity.fqn)
				entityCopied.resolve(resolver=newResolver)

				dependencies.intra.push(entityCopied)

		return dependencies

	def __str__(self) -> str:
		content = []
		for entity, dependencies in self.map.items():
			content += [str(entity)] + [f"\t{line}" for line in str(dependencies).split("\n")]
		return "\n".join(content)
