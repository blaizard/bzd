import typing
import pathlib

from bzd.parser.error import Error

from tools.bdl.visitor import CATEGORY_GLOBAL_COMPOSITION
from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.object import Object
from tools.bdl.entities.impl.entity import Entity
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.impl.method import Method
from tools.bdl.entities.impl.nested import Nested, TYPE_INTERFACE
from tools.bdl.entities.impl.fragment.fqn import FQN
from tools.bdl.entities.builder import MethodBuilder, NestedBuilder

# Add builtins
MethodComponentInit = Method(MethodBuilder(name="init").get())
NestedComponent = Nested(
	NestedBuilder(kind=TYPE_INTERFACE).pushBackElementToNestedSequence("interface", MethodComponentInit.element).get())

GLOBAL_FQN_OUT = "out"
GLOBAL_FQN_IN = "in"
GLOBAL_FQN_STEADY_CLOCK = "steadyClock"
GLOBAL_FQN_SYSTEM_CLOCK = "systemClock"

class Composition:

	def __init__(self, includes: typing.Optional[typing.List[pathlib.Path]] = None) -> None:
		self.includes = [] if includes is None else includes
		self.symbols = SymbolMap()
		self.registry: typing.List[Expression] = []
		self.registryFQNs: typing.Set[str] = set()
		self.compositions: typing.Dict[str, typing.List[Expression]] = {"init": [], "compose": []}
		self.executors: typing.Set[str] = set()

	def visit(self, bdl: Object) -> "Composition":

		# Build a master symbol list
		self.symbols.update(bdl.symbols)
		return self

	def resolveDependency(self,
		dependencies: typing.Dict[str, typing.Set[str]],
		fqn: str,
		resolved: typing.List[str],
		unresolved: typing.Optional[typing.Set[str]] = None) -> None:

		if unresolved is None:
			unresolved = set()

		unresolved.add(fqn)
		for dep in dependencies[fqn]:
			if dep not in resolved:
				if dep in unresolved:
					entity1 = self.symbols.getEntityResolved(fqn=fqn).value
					entity2 = self.symbols.getEntityResolved(fqn=dep).value
					message = entity1.error(message="Circular dependency detected between this expression...",
						throw=False)
					message += entity2.error(message="...and this one.", throw=False)
					raise Exception(message)
				self.resolveDependency(dependencies, dep, resolved, unresolved)
		if fqn not in resolved:
			resolved.append(fqn)
		unresolved.remove(fqn)

	def orderDependencies(self, entities: typing.Iterator[typing.Tuple[str, Entity]]) -> typing.List[str]:
		dependencies: typing.Dict[str, typing.Set[str]] = {}
		for fqn, entity in entities:

			# Unamed entries should be dealt with later.
			if not entity.isName:
				continue

			resolver = self.symbols.makeResolver(namespace=entity.namespace)

			dependencies[fqn] = {
				resolver.resolveShallowFQN(name=dep).assertValue(element=entity.element)[0]
				for dep in entity.dependencies
			}

		# Remove dependencies that are irrelevant
		dependencies = {fqn: deps.intersection(dependencies.keys()) for fqn, deps in dependencies.items()}

		# Compute the dependency orders and identify circular dependencies.
		orderFQNs: typing.List[str] = []
		orderKeys = sorted(dependencies.keys(), key=lambda key: {GLOBAL_FQN_OUT: 1, GLOBAL_FQN_IN: 2, GLOBAL_FQN_STEADY_CLOCK: 3, GLOBAL_FQN_SYSTEM_CLOCK: 4}.get(key, 999))
		for fqn in orderKeys:
			self.resolveDependency(dependencies, fqn, orderFQNs)

		return orderFQNs

	def addComposition(self, entity: Expression) -> None:
		"""
		Add a new composition entity to the compisiton list.
		"""
		self.compositions["compose"].append(entity)

		# Update the executor list
		self.executors.add(entity.executor)

	def addInit(self, fqn: str, entity: typing.Optional[Expression] = None) -> None:
		"""
		Add a new composition initializer entry.
		"""
		self.compositions["init"].append({
			"fqn": fqn,
			"isCall": entity is not None,
			"call": entity
		})

		# Update the executor list
		if entity:
			self.executors.add(entity.executor)

	def process(self) -> None:

		categories = {CATEGORY_GLOBAL_COMPOSITION}

		# Make the dependency graph
		orderedFQNs = self.orderDependencies(entities=self.symbols.items(categories=categories))
		self.registry = [self.symbols.getEntityResolved(fqn=fqn).value for fqn in orderedFQNs]  # type: ignore
		self.registryFQNs = set(orderedFQNs)

		# Resolve the Registry and then the un-named
		for entity in self.registry:
			resolver = self.symbols.makeResolver(namespace=entity.namespace)

			entity.resolveMemoized(resolver=resolver)

			# Identify entities that contains nested composition
			entityUnderlyingType = entity.getEntityUnderlyingTypeResolved(resolver=resolver)
			if entityUnderlyingType.isComposition:
				for compositionEntity in entityUnderlyingType.composition:
					compositionEntity.assertTrue(condition=not compositionEntity.isName,
						message="Variable cannot be created within a nested composition.")

					# Create new entities and associate them with their respective objects.
					entityCopied = compositionEntity.copy()
					resolver = self.symbols.makeResolver(namespace=entityCopied.namespace, this=entity.fqn)
					entityCopied.resolve(resolver=resolver)

					assert isinstance(entityCopied, Expression)
					self.addInit(fqn=entity.fqn, entity=entityCopied)

			else:
				self.addInit(fqn=entity.fqn)

		# resolve the un-named
		self.executors = set()
		for fqn, entity in self.symbols.items(categories=categories):  # type: ignore
			if entity.isName:
				continue
			assert isinstance(entity, Expression)

			entity.resolveMemoized(resolver=self.symbols.makeResolver(namespace=entity.namespace))

			# Update any variables if part of the registry
			if entity.isInit:
				self.addInit(fqn=entity.fqn, entity=entity)
			else:
				self.addComposition(entity=entity)

		# Ensure all executors are declared.
		executorsIntersection = self.executors.intersection(self.registryFQNs)
		assert executorsIntersection == self.executors, "Some executors are used but not declared: {}".format(", ".join(
			self.executors.difference(executorsIntersection)))
