import typing
import dataclasses

from tools.bdl.visitor import Group
from tools.bdl.visitors.symbol_map import SymbolMap, Resolver
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
	# Dependencies over other entities, first level dependencies.
	deps: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	# Direct dependencies.
	init: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	intra: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	shutdown: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)

	def __repr__(self) -> str:
		content = []
		content += [f"executor: {str(self.executor)}"]
		content += [f"deps: {self.deps}"]
		content += [f"init: {self.init}"]
		content += [f"intra: {self.intra}"]
		content += [f"shutdown: {self.shutdown}"]
		return "\n".join(content)


class Entities:
	"""Class to handle entities and their dependencies."""

	def __init__(self, symbols: SymbolMap) -> None:
		self.symbols = symbols
		# Map of all available components and their dependencies.
		self.components: typing.Dict[Entity, Dependencies] = {}
		# Map of all available connections.
		self.connections: typing.Dict[Entity, typing.Set[Entity]] = {}

	def findAllIntra(self, entities: typing.List[Entity]) -> DependencyGroup:
		"""Find all intra expressions associated with this entities."""

		dependencies = DependencyGroup()
		for entity in entities:
			assert entity in self.components, f"The entity {str(entity)} is not registered in the map."
			deps = self.findAllIntra([*self.components[entity].deps])
			dependencies.pushGroup(deps)
			dependencies.pushGroup([*self.components[entity].intra])

		return dependencies

	def processMeta(self, entity: Entity) -> None:
		"""Process a meta expression, a special function from the language."""

		entity.assertTrue(condition=entity.isSymbol, message=f"Meta expressions must have a symbol.")
		if entity.symbol.fqn == "connect":

			io1 = entity.parametersResolved[0].param
			io2 = entity.parametersResolved[1].param

			io1.assertTrue(condition=io1.isLValue, message="First argument must be a reference to another object.")
			io2.assertTrue(condition=io2.isLValue, message="Second argument must be a reference to another object.")

			entity.assertTrue(io1 not in self.connections or io2 not in self.connections[io1],
				f"Connection between '{io1}' and '{io2}' is defined multiple times.")
			self.connections.setdefault(io1, set()).add(io2)
			entity.assertTrue(io2 not in self.connections or io1 not in self.connections[io2],
				f"Connection between '{io1}' and '{io2}' is defined multiple times.")
			self.connections.setdefault(io2, set()).add(io1)

		else:
			entity.error(message="Unsupported meta expression.")

	def getWorkloads(self) -> typing.Iterable[Entity]:
		"""List all workloads.
		Workload are the entities with no name.
		"""
		for entity in self.components.keys():
			if not entity.isName:
				yield entity

	def getRegistry(self) -> typing.Iterable[typing.Tuple[str, Entity]]:
		"""The registry."""
		for entity in self.components.keys():
			if entity.isName:
				yield entity.fqn, entity

	def getPlatform(self) -> typing.Iterable[typing.Tuple[str, Entity]]:
		"""The platform entities."""
		for entity in self.components.keys():
			if entity.isName and entity.fqn.startswith("platform"):
				yield entity.fqn, entity

	def update(self, entities: typing.List[Entity]) -> None:
		"""Update the object with new entities."""

		for entity in entities:
			self.add_(entity)

	def add_(self, entity: Entity) -> None:
		"""Add a single entity."""

		# This can happen when new entries are added from the implicit dependencies.
		if entity in self.components:
			return

		# Resolve the entity against its namespace.
		resolver = self.symbols.makeResolver(namespace=entity.namespace)
		entity.resolveMemoized(resolver=resolver)

		if entity.isRoleMeta:
			self.processMeta(entity)
		else:
			dependencies = self.resolveDependencies(entity=entity, resolver=resolver)
			self.components[entity] = dependencies

			# Add implicit dependencies.
			for dependency in dependencies.deps + dependencies.intra:
				self.add_(dependency)

	def resolveDependencies(self, entity: Entity, resolver: Resolver) -> Dependencies:
		"""Resolve the dependencies for a specific entity."""

		# These corresponds to the entities to be executed before / during / after.
		dependencies = Dependencies()

		def checkIfInitOrShutdown(interfaceEntity: Entity) -> typing.Optional[DependencyGroup]:
			if interfaceEntity.contracts.has("init"):
				return dependencies.init
			if interfaceEntity.contracts.has("shutdown"):
				return dependencies.shutdown
			return None

		if isinstance(entity, Expression):

			if entity.executor:
				dependencies.executor = self.symbols.getEntityResolved(fqn=entity.executor).assertValue(
					element=entity.element)

			# Look for all dependencies and add the expression only.
			for fqn in entity.dependencies:
				dep = self.symbols.getEntityResolved(fqn=fqn).value
				if isinstance(dep, Expression):
					dependencies.deps.push(dep)

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
						group=Group.composition)
					newEntity = self.symbols.getEntityResolved(fqn=fqn).value
					newResolver = self.symbols.makeResolver(namespace=interfaceEntity.namespace, this=entity.fqn)
					newEntity.resolveMemoized(resolver=newResolver)
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
				entityCopied.resolveMemoized(resolver=newResolver)

				dependencies.intra.push(entityCopied)

		return dependencies

	def __str__(self) -> str:
		content = ["==== Components ===="]
		for entity, dependencies in self.components.items():
			content += [str(entity)] + [f"\t{line}" for line in str(dependencies).split("\n")]
		content += ["==== Connections ===="]
		for io, setOfIOs in self.connections.items():
			content += [f"{io} => {setOfIOs}"]

		return "\n".join(content)
