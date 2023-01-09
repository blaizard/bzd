import typing
import dataclasses
from functools import cached_property

from tools.bdl.visitor import Group
from tools.bdl.visitors.symbol_map import SymbolMap, Resolver
from tools.bdl.entities.impl.builtin import Builtin
from tools.bdl.entities.impl.entity import Entity, EntityExpression
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.builder import ExpressionBuilder
from tools.bdl.entities.impl.fragment.symbol import Symbol


class DependencyGroup:
	"""List of ordered dependencies."""

	def __init__(self) -> None:
		self.data: typing.Set[Expression] = set()

	def copy(self) -> "DependencyGroup":
		"""Create a copy of this object."""
		dependencies = DependencyGroup()
		dependencies.data = self.data.copy()
		return dependencies

	def push(self, expression: Expression) -> None:
		"""Add a new dependency to the list."""
		# Ignore Builtin dependencies, as they are expected to be available at all time.
		if isinstance(expression, Builtin):
			return
		if expression in self.data:
			return
		self.data.add(expression)

	def pushGroup(self, group: typing.Union["DependencyGroup", typing.List[Expression]]) -> None:
		"""Push an ordered group of dependency to the list."""
		for expression in group:
			self.push(expression)

	def isSubset(self, entities: typing.Iterable[Expression]) -> bool:
		"""Check if this group is contained in an iterable of expressions,
		assuming the iterable only contains unique entries."""

		count = len(self.data)
		for entity in entities:
			if entity in self.data:
				count -= 1
		return count == 0

	def __add__(self, other: "DependencyGroup") -> "DependencyGroup":
		dependencies = self.copy()
		dependencies.pushGroup(other)
		return dependencies

	def __iter__(self) -> typing.Iterator[Expression]:
		for expression in self.data:
			yield expression

	def __len__(self) -> int:
		return len(self.data)

	def __repr__(self) -> str:
		return ", ".join([str(e) for e in self.data])


@dataclasses.dataclass
class Dependencies:
	executor: typing.Optional[Expression] = None
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


class Connections:

	def __init__(self) -> None:
		self.map: typing.Dict[Symbol, typing.Set[Symbol]] = {}

	def add(self, io1: EntityExpression, io2: EntityExpression) -> None:
		"""Register a new connection to the connection map."""

		io1.assertTrue(condition=io1.isLValue, message="First argument must be a reference to another object.")
		io2.assertTrue(condition=io2.isLValue, message="Second argument must be a reference to another object.")

		io1.assertTrue(condition=io1.symbol != io2.symbol, message="A connection cannot connect to itself.")
		io1.assertTrue(condition=io1.underlyingTypeFQN == io2.underlyingTypeFQN,
			message=
			f"Connections cannot only be made between same types, not {io1.underlyingTypeFQN} and {io2.underlyingTypeFQN}."
						)

		alreadyInserted = io1.symbol in self.map and io2.symbol in self.map[io1.symbol]
		io1.assertTrue(condition=not alreadyInserted,
			message=f"Connection between '{io1.symbol}' and '{io2.symbol}' is defined multiple times.")
		self.map.setdefault(io1.symbol, set()).add(io2.symbol)
		self.map.setdefault(io2.symbol, set()).add(io1.symbol)

	def __repr__(self) -> str:
		content = []
		for io, setOfIOs in self.map.items():
			content += [f"{io} => {setOfIOs}"]
		return "\n".join(content)


class Entities:
	"""Class to handle entities and their dependencies."""

	def __init__(self, symbols: SymbolMap) -> None:
		self.symbols = symbols
		# Map of all available components and their dependencies.
		self.components: typing.Dict[Expression, Dependencies] = {}
		# Map of all available connections.
		self.connections = Connections()

	def findAllIntra(self, entities: typing.List[Entity]) -> DependencyGroup:
		"""Find all intra expressions associated with this entities."""

		dependencies = DependencyGroup()
		for entity in entities:
			assert entity in self.components, f"The entity {str(entity)} is not registered in the map."
			deps = self.findAllIntra([*self.components[entity].deps])
			dependencies.pushGroup(deps)
			dependencies.pushGroup([*self.components[entity].intra])

		return dependencies

	def processMeta(self, expression: Expression) -> None:
		"""Process a meta expression, a special function from the language."""

		expression.assertTrue(condition=expression.isSymbol, message=f"Meta expressions must have a symbol.")
		if expression.symbol.fqn == "connect":
			self.connections.add(expression.parametersResolved[0].param, expression.parametersResolved[1].param)

		else:
			expression.error(message="Unsupported meta expression.")

	def getWorkloads(self) -> typing.Iterable[Expression]:
		"""List all workloads.
		Workload are the entities with no name.
		"""
		for expression in self.components.keys():
			if self.isWorkload(expression):
				yield expression

	def resolveDependencies(self, entities: typing.List[Expression]) -> typing.List[Expression]:
		"""From entities that are part of self.components, resolve the dependencies to ensure
		that each entity can be create sequentially with only previously created entites as dependencies."""

		resolved: typing.List[Expression] = []

		while entities:

			remaining: typing.List[Expression] = []
			for entity in entities:
				if self.components[entity].deps.isSubset(resolved):
					resolved.append(entity)
				else:
					remaining.append(entity)

			if len(entities) == len(remaining):
				entities[0].error(message="The dependencies of this expression are not met.")

			entities = remaining

		return resolved

	@cached_property
	def registry(self) -> typing.Iterable[Expression]:
		"""The registry."""

		return self.resolveDependencies([e for e in self.components.keys() if self.isRegistry(e)])

	@property
	def platform(self) -> typing.Iterable[typing.Tuple[str, Expression]]:
		"""The platform entities."""
		for expression in self.components.keys():
			if self.isPlatform(expression):
				yield expression.fqn, expression

	@staticmethod
	def isWorkload(expression: Expression) -> bool:
		return not expression.isName

	@staticmethod
	def isRegistry(expression: Expression) -> bool:
		return expression.isName

	@staticmethod
	def isPlatform(expression: Expression) -> bool:
		return expression.isName and expression.fqn.startswith("platform")

	def add(self, entity: Entity, isDep: bool = False) -> None:
		"""Add a new entity."""

		# This can happen when new entries are added from the implicit dependencies.
		if entity in self.components:
			return

		entity.assertTrue(condition=isinstance(entity, Expression),
			message="Only expressions can be added to the composition.")
		expression = typing.cast(Expression, entity)

		# Resolve the entity against its namespace.
		resolver = self.symbols.makeResolver(namespace=expression.namespace)
		expression.resolveMemoized(resolver=resolver)

		if not (isDep or self.isWorkload(entity) or self.isPlatform(entity)):
			return

		if expression.isRoleMeta:
			self.processMeta(expression=expression)
		else:
			self.processDependencies(expression=expression, resolver=resolver)

	def processDependencies(self, expression: Expression, resolver: Resolver) -> None:
		"""Resolve the dependencies for a specific expression."""

		# These corresponds to the entities to be executed before / during / after.
		self.components[expression] = Dependencies()

		def checkIfInitOrShutdown(interfaceEntity: Entity) -> typing.Optional[DependencyGroup]:
			if interfaceEntity.contracts.has("init"):
				return self.components[expression].init
			if interfaceEntity.contracts.has("shutdown"):
				return self.components[expression].shutdown
			return None

		# Set the executor, it is guarantee by the Expressiontype that there is always an executor.
		self.components[expression].executor = self.symbols.getEntityResolved(fqn=expression.executor).assertValue(
			element=expression.element)
		self.add(self.components[expression].executor, isDep=True)

		# Look for all dependencies and add the expression only.
		for fqn in expression.dependencies:
			dep = self.symbols.getEntityResolved(fqn=fqn).value
			if isinstance(dep, Expression):
				self.components[expression].deps.push(dep)

		underlyingType = expression.getEntityUnderlyingTypeResolved(resolver=resolver)

		# Check if there are pre/post-requisites (init/shutdown functions)
		if underlyingType.isInterface:
			for interfaceEntity in underlyingType.interface:
				maybeGroup = checkIfInitOrShutdown(interfaceEntity)
				if maybeGroup is not None:

					# Insert a new entry in the symbol map and resolve it against this entity.
					fqn = self.symbols.insert(name=None,
						namespace=interfaceEntity.namespace,
						path=None,
						element=ExpressionBuilder(type=f"this.{interfaceEntity.name}"),
						group=Group.composition)
					newEntity = self.symbols.getEntityResolved(fqn=fqn).value
					newResolver = self.symbols.makeResolver(namespace=interfaceEntity.namespace, this=expression.fqn)
					newEntity.resolveMemoized(resolver=newResolver)
					maybeGroup.push(newEntity)

		# Check if there are dependent composition from this entity.
		if underlyingType.isComposition:
			for compositionEntity in underlyingType.composition:
				# Composition must only contain unamed entries.
				compositionEntity.assertTrue(condition=not compositionEntity.isName,
					message="Variable cannot be created within a nested composition.")
				assert isinstance(compositionEntity, Expression), "All composition entities must be an expression."

				# Create a new entity and associate it with its respective objects.
				entityCopied = compositionEntity.copy()
				newResolver = self.symbols.makeResolver(namespace=entityCopied.namespace, this=expression.fqn)
				entityCopied.resolveMemoized(resolver=newResolver)

				self.components[expression].intra.push(entityCopied)

		# Add implicit dependencies.
		for dependency in self.components[expression].deps + self.components[expression].intra:
			self.add(dependency, isDep=True)

	def __str__(self) -> str:
		content = ["==== Components ===="]
		for expression, dependencies in self.components.items():
			content += [str(expression)] + [f"\t{line}" for line in str(dependencies).split("\n")]
		content += ["==== Connections ===="]
		content += [str("\n".join([f"\t- {c}" for c in str(self.connections).split("\n")]))]

		return "\n".join(content)
