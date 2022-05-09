import typing
import pathlib
import dataclasses

from bzd.parser.error import Error

from tools.bdl.visitor import CATEGORY_GLOBAL_COMPOSITION, CATEGORY_COMPOSITION
from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.visitors.composition.dependency_map import DependencyMap
from tools.bdl.object import Object
from tools.bdl.entities.impl.entity import Entity
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.impl.method import Method
from tools.bdl.entities.impl.nested import Nested, TYPE_INTERFACE
from tools.bdl.entities.impl.fragment.fqn import FQN

GLOBAL_FQN_OUT = "out"
GLOBAL_FQN_IN = "in"
GLOBAL_FQN_STEADY_CLOCK = "steadyClock"
GLOBAL_FQN_SYSTEM_CLOCK = "systemClock"


@dataclasses.dataclass(frozen=True)
class AsyncType:
	active = "active"
	service = "service"


class Composition:

	def __init__(self, includes: typing.Optional[typing.List[pathlib.Path]] = None) -> None:

		self.infraFQNs: typing.Final[typing.List[str]] = ["out", "in", "steadyClock", "systemClock"]
		self.includes = [] if includes is None else includes
		self.symbols = SymbolMap()
		self.dependencies = DependencyMap(symbols=self.symbols)
		self.registry: typing.Dict[str, Expression] = {}
		self.initialization: typing.List[typing.Dict[str, typing.Any]] = []
		self.composition: typing.List[typing.Dict[str, typing.Any]] = []

		# All top level expressions
		self.all: typing.Dict[str, Expression] = {}
		# All infrastructure dependencies.
		self.infra: typing.List[Expression] = []
		# All executors.
		self.executors: typing.Dict[str, Expression] = {}
		# All composition per executors.
		self.comp: typing.Dict[str, typing.Dict[Entity, str]] = {}

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

	def orderDependencies(self, entities: typing.Dict[str, Entity]) -> typing.List[str]:
		dependencies: typing.Dict[str, typing.Set[str]] = {}
		for fqn, entity in entities.items():

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
		orderKeys = sorted(dependencies.keys(),
			key=lambda key: {
			GLOBAL_FQN_OUT: 1,
			GLOBAL_FQN_IN: 2,
			GLOBAL_FQN_STEADY_CLOCK: 3,
			GLOBAL_FQN_SYSTEM_CLOCK: 4
			}.get(key, 999))
		for fqn in orderKeys:
			self.resolveDependency(dependencies, fqn, orderFQNs)

		return orderFQNs

	def addExecutor(self, entity: Expression) -> None:
		"""Add a new executor from the given expression."""

		entity.assertTrue(entity.executor in self.registry, f"The executor '{entity.executor}' is not declared.")
		self.executors[entity.executor] = self.registry[entity.executor]

	def addComposition(self, entity: Expression, asyncType: str) -> None:
		"""
		Add a new composition entity to the compisiton list.
		"""
		self.composition.append({"entity": entity, "asyncType": asyncType})

		# Update the executor list
		self.addExecutor(entity)

	def addInit(self, fqn: str, entity: typing.Optional[Expression] = None) -> None:
		"""
		Add a new composition initializer entry.
		"""
		self.initialization.append({"fqn": fqn, "isCall": entity is not None, "call": entity})

		# Update the executor list
		if entity:
			self.addExecutor(entity)

	def process(self) -> None:

		categories = {CATEGORY_GLOBAL_COMPOSITION}
		self.all = {fqn: expression for fqn, expression in self.symbols.items(categories=categories)} # type: ignore

		# Make the dependency graph
		orderedFQNs = self.orderDependencies(entities=self.all) # type: ignore
		self.registry = {fqn: self.symbols.getEntityResolved(fqn=fqn).value for fqn in orderedFQNs}  # type: ignore
		self.executors = {}

		tasks = []

		# Resolve the Registry.
		# This handles components, from which an instance must be defined.
		for entity in self.registry.values():

			self.dependencies.add(entity)
			if not entity.isName:
				tasks.append(entity)

			resolver = self.symbols.makeResolver(namespace=entity.namespace)

			entity.resolveMemoized(resolver=resolver)

			entityUnderlyingType = entity.getEntityUnderlyingTypeResolved(resolver=resolver)
			# Identify entities that contains nested composition
			if entityUnderlyingType.isComposition:
				for compositionEntity in entityUnderlyingType.composition:
					compositionEntity.assertTrue(condition=not compositionEntity.isName,
						message="Variable cannot be created within a nested composition.")

					# Create new entities and associate them with their respective objects.
					entityCopied = compositionEntity.copy()
					resolver = self.symbols.makeResolver(namespace=entityCopied.namespace, this=entity.fqn)
					entityCopied.resolve(resolver=resolver)

					assert isinstance(entityCopied, Expression)
					if entityCopied.isInit:
						self.addInit(fqn=entity.fqn, entity=entityCopied)
					else:
						self.addComposition(entity=entityCopied, asyncType=AsyncType.service)

			else:
				self.addInit(fqn=entity.fqn)

		# Resolve the un-named.
		# This handles free function calls.
		for fqn, entity in self.symbols.items(categories=categories):  # type: ignore
			if entity.isName:
				continue
			tasks.append(entity)

			self.dependencies.add(entity)

			assert isinstance(entity, Expression)
			entity.resolveMemoized(resolver=self.symbols.makeResolver(namespace=entity.namespace))

			# Update any variables if part of the registry
			self.addComposition(entity=entity, asyncType=AsyncType.active)

		self.dependencies.addImplicit()

		#for entity, deps in self.dependencies.map.items():
		#	print("DEPS FOR", entity)
		#	print(self.dependencies.map[entity].preDeps)
		#	print(self.dependencies.buildList(entity))
		#print(self.dependencies)

		self.infra = [self.registry[fqn] for fqn in self.infraFQNs if fqn in self.registry]

		# Applications are all intra expressions that are instanciated at top level
		self.comp = {}
		for fqn, entity in self.all.items():
			if entity.isName:
				continue
			entity.assertTrue(entity.executor in self.registry, f"The executor '{entity.executor}' is not declared.")
			self.comp.setdefault(entity.executor, dict())[entity] = AsyncType.service

		# Services are all intra expressions that are deps from all tasks, associated executor and infra.
		commonServices = self.dependencies.findAllIntra(self.infra) # type: ignore
		for fqn, executorComposition in self.comp.items():
			services = commonServices + self.dependencies.findAllIntra(
				[self.registry[fqn]]) + self.dependencies.findAllIntra([*executorComposition.keys()])
			executorComposition.update({entity: AsyncType.active for entity in services})

		#print(self.comp)
		#print(self.all)

		#all intra for infra:
		#
		# Run service
		#
		#for task in tasks:
		#
		# Run app
		# []() -> Async<> {
		#	co_await task.init()
		#	co_await task.exec()
		#	co_await task.shutdown()
		# }
		#
		#
		#	self.dependencies.map[task].deps
		#print(tasks)

	def __str__(self) -> str:
		"""Print a human readable view of this instance."""

		def addContent(content: typing.List[str], title: str, contentList: typing.Iterable[typing.Any]) -> None:
			content.append(f"==== {title} ====")
			content += [f"\t- {item}" for item in contentList]

		content: typing.List[str] = []
		addContent(content, "Includes", self.includes)
		addContent(content, "Symbols", str(self.symbols).split("\n"))
		addContent(content, "Dependencies", str(self.dependencies).split("\n"))
		addContent(content, "Registry", self.registry.values())
		addContent(content, "Executors", self.executors.keys())
		addContent(content, "Initialization", self.initialization)
		addContent(content, "Composition", self.composition)

		return "\n".join(content)
