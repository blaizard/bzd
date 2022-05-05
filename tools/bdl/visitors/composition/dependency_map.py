import typing
import dataclasses

from tools.bdl.visitor import CATEGORY_COMPOSITION
from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.entities.impl.entity import Entity
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.builder import ExpressionBuilder


@dataclasses.dataclass
class Dependencies:
	executor: typing.Optional[Entity] = None
	# Dependencies over other entities
	preDeps: typing.List[Entity] = dataclasses.field(default_factory=list)
	# Direct dependencies
	pre: typing.List[Entity] = dataclasses.field(default_factory=list)
	intra: typing.List[Entity] = dataclasses.field(default_factory=list)
	postDeps: typing.List[Entity] = dataclasses.field(default_factory=list)
	post: typing.List[Entity] = dataclasses.field(default_factory=list)

	def allDeps(self) -> typing.List[Entity]:
		return self.preDeps + self.postDeps

	def __str__(self) -> str:
		content = []
		content += [f"executor: {str(self.executor)}"]
		content += [f"preDeps: {', '.join([str(e) for e in self.preDeps])}"]
		content += [f"pre: {', '.join([str(e) for e in self.pre])}"]
		content += [f"intra: {', '.join([str(e) for e in self.intra])}"]
		content += [f"postDeps: {', '.join([str(e) for e in self.postDeps])}"]
		content += [f"post: {', '.join([str(e) for e in self.post])}"]
		return "\n".join(content)


class DependencyMap:
	"""Class to handle entities and their dependencies."""

	def __init__(self, symbols: SymbolMap) -> None:
		self.symbols = symbols
		self.map: typing.Dict[Entity, Dependencies] = {}

	def buildList(self, entity: Entity) -> typing.List[Entity]:
		"""Build the list of dependencies for a specific entity.
		The list contains ordered dependencies from the first to the last, in the same order
		in which they should be processed.
		"""

		assert entity in self.map, f"The entity {str(entity)} is not registered in the map."

		def mergeLists(dependencyList: typing.List[Entity], listToMerge: typing.List[Entity]) -> typing.List[Entity]:
			return dependencyList + [e for e in listToMerge if e not in dependencyList]

		dependencyList: typing.List[Entity] = []
		for dep in self.map[entity].preDeps:
			dependencyList = mergeLists(dependencyList, self.buildList(entity=dep))
		dependencyList = mergeLists(dependencyList, self.map[entity].pre)

		return dependencyList

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
				for dependency in dependencies.allDeps():
					if dependency not in self.map and dependency not in updated:
						updated[dependency] = self.resolveDependencies(dependency)
			if len(updated) == 0:
				break
			self.map.update(updated)

	def resolveDependencies(self, entity: Entity) -> Dependencies:
		"""Resolve the dependencies for a specific entity."""

		def checkIfInitOrShutdown(interfaceEntity: Entity) -> typing.Optional[typing.List[Entity]]:
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
				dependencies.preDeps += [
					self.symbols.getEntityResolved(fqn=fqn).value for fqn in entity.parameters.dependencies
				]

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
					maybeGroup.append(newEntity)

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

				dependencies.intra.append(entityCopied)

		return dependencies

	def __str__(self) -> str:
		content = []
		for entity, dependencies in self.map.items():
			content += [str(entity)] + [f"\t{line}" for line in str(dependencies).split("\n")]
		return "\n".join(content)
