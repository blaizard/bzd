import typing
import dataclasses
import enum
from functools import cached_property
from collections import OrderedDict

from bzd.utils.result import Result

from tools.bdl.visitor import Group
from tools.bdl.visitors.symbol_map import SymbolMap, Resolver
from tools.bdl.entities.impl.builtin import Builtin
from tools.bdl.entities.impl.entity import Entity, EntityExpression
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.builder import ExpressionBuilder
from tools.bdl.entities.impl.fragment.symbol import Symbol
from tools.bdl.entities.impl.types import Category


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
class OutputMetadata:
	# The FQN of the executor associated with this connection.
	executor: str
	# The maximum history required by this connection.
	history: int = 1


@dataclasses.dataclass
class ConnectionGroup:
	# The FQN of the executor associated with this input.
	executor: str
	# The type of symbol for this connection group.
	symbol: typing.Optional[Symbol] = None
	# The set of connection object for this group.
	outputs: typing.Dict[Symbol, OutputMetadata] = dataclasses.field(default_factory=dict)


class Connections:

	def __init__(self) -> None:
		self.outputs: typing.Set[Symbol] = set()
		self.groups: typing.Dict[Symbol, ConnectionGroup] = {}

	def add(self, input: EntityExpression, output: EntityExpression) -> None:
		"""Register a new connection to the connection map."""

		input.assertTrue(condition=input.isLValue, message="First argument must be a reference to another object.")
		output.assertTrue(condition=output.isLValue, message="Second argument must be a reference to another object.")

		input.assertTrue(condition=input.symbol != output.symbol, message="A connection cannot connect to itself.")
		input.assertTrue(condition=input.underlyingTypeFQN == output.underlyingTypeFQN,
			message=
			f"Connections cannot only be made between same types, not {input.underlyingTypeFQN} and {output.underlyingTypeFQN}."
							)

		alreadyInserted = input.symbol in self.groups and output.symbol in self.groups[input.symbol].outputs
		input.assertTrue(condition=not alreadyInserted,
			message=f"Connection between '{input.symbol}' and '{output.symbol}' is defined multiple times.")
		input.assertTrue(condition=input.symbol not in self.outputs,
			message=f"'{input.symbol}' has already been defined as an output.")
		if input.symbol not in self.outputs:
			self.groups[input.symbol] = ConnectionGroup(executor=input.executorOr("executor"))
		self.groups[input.symbol].outputs[output.symbol] = OutputMetadata(executor=output.executorOr("executor"))
		self.outputs.add(output.symbol)

	def resolve(self, resolver: Resolver) -> None:
		for symbol, group in self.groups.items():
			group.symbol = symbol.getEntityUnderlyingTypeResolved(resolver=resolver).symbol

	def __repr__(self) -> str:
		content = []
		for input, connection in self.groups.items():
			content += [f"{input} => {connection.outputs.keys()}"]
		return "\n".join(content)


class EntryType(enum.Flag):
	# Items that are part of the registry, should be created before running any asyncs.
	registry = enum.auto()
	# Worload type of asyncs, define the lifetime of the application.
	workload = enum.auto()
	# Service type of asyncs, are stopped when no workloads are running.
	service = enum.auto()
	# Part of the platform declaration.
	platform = enum.auto()
	# Executor type, all executors must also be part of the registry.
	executor = enum.auto()


@dataclasses.dataclass
class ExpressionEntry:
	# The expression itself.
	expression: Expression
	# The asynchronous type of this entity.
	entryType: EntryType
	# The executors associated with this entry.
	executors: typing.Set[str] = dataclasses.field(default_factory=set)
	# Dependencies over other entities, first level dependencies.
	deps: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	# Direct dependencies.
	init: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	intra: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	shutdown: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)

	def __repr__(self) -> str:
		content = [
			f"type: {str(self.entryType)}", f"expression: {str(self.expression)}", f"executors: {str(self.executors)}",
			f"deps: {self.deps}", f"init: {self.init}", f"intra: {self.intra}", f"shutdown: {self.shutdown}"
		]
		return "\n".join(content)


class Components:

	def __init__(self) -> None:
		self.map: typing.Dict[str, ExpressionEntry] = OrderedDict()

	@staticmethod
	def makeId(expression: Expression) -> str:
		"""Create an identifier from an expression.
		An Identifier is created with the name and the symbol or value.
		"""

		result = f"{expression.fqn}|" if expression.isName else "|"
		result += expression.value if expression.isValue else str(expression.symbol.fqn)
		return result

	def resolve(self) -> None:
		"""Resolve the dependencies to ensure that each entry can be created sequentially
		with only previously created entries as dependencies."""

		def isSatisfied(group: DependencyGroup) -> bool:
			"""Check that the group dependency is statified with the current map."""

			for expression in group:
				if self.makeId(expression) not in self.map:
					return False
			return True

		entries = [(k, v) for k, v in self.map.items()]
		self.map: typing.Dict[str, ExpressionEntry] = OrderedDict()

		while entries:

			remaining: typing.List[typing.Tuple[str, ExpressionEntry]] = []
			for identifier, entry in entries:
				if isSatisfied(entry.deps):
					self.map[identifier] = entry
				else:
					remaining.append((identifier, entry))

			if len(entries) == len(remaining):
				entries[0][0].expression.error(message="The dependencies of this expression are not met.")

			entries = remaining

	def close(self) -> None:
		self.resolve()

	def __contains__(self, expression: Expression) -> bool:
		"""Check if an entry already exists in the map."""

		return self.makeId(expression) in self.map

	def __getitem__(self, expression: Expression) -> Result[ExpressionEntry, str]:
		"""Get a specific item."""

		identifier = self.makeId(expression)
		if identifier not in self.map:
			return Result.makeError(f"The expression with the identifier '{identifier}' is not registered in the map.")
		return Result(self.map[identifier])

	def __iter__(self) -> typing.Iterator[ExpressionEntry]:
		for entry in self.map.values():
			yield entry

	def insert(self, expression: Expression, entryType: EntryType, executor: str) -> ExpressionEntry:
		"""Insert a new entry in the map, return the entry if it does not exists or if it can be updated,
		otherwise it returns None."""

		identifier = self.makeId(expression)

		# Ensure that only a workload can superseed an existing expression with the same id.
		if identifier in self.map:

			def isOverwritable(entryTypeFrom: EntryType, entryTypeTo: EntryType) -> bool:
				return (EntryType.service in entryTypeFrom) and (EntryType.workload in entryTypeTo)

			# If this is the exact same expression.
			if self.map[identifier].entryType == entryType:
				expression.assertTrue(condition=(self.map[identifier].expression == expression),
					message=f"This expression already exits and cannot be redeclared as the same role: '{entryType}'.")
				return None
			if isOverwritable(entryType, self.map[identifier].entryType):
				return None
			expression.assertTrue(condition=isOverwritable(self.map[identifier].entryType, entryType),
				message=
				f"An expression of role '{entryType}' cannot overwrite an expression of role '{self.map[identifier].entryType}'."
									)

		self.map[identifier] = ExpressionEntry(expression=expression, entryType=entryType, executors={executor})
		return self.map[identifier]

	def getDependencies(self, expression: Expression) -> typing.Iterator[ExpressionEntry]:
		"""Follow the dependencies of a particular expression and output each if its entries including itself."""

		alreadyVisited: typing.Set[str] = set()
		toVisit = [expression]
		while toVisit:
			expression = toVisit.pop(0)
			identifier = self.makeId(expression)
			if identifier not in alreadyVisited:
				alreadyVisited.add(identifier)
				if identifier in self.map:
					entry = self.map[identifier]
					yield entry
					toVisit += [*entry.deps, *entry.init, *entry.intra, *entry.shutdown]

	def __repr__(self) -> str:
		content = []
		for identifier, entry in self.map.items():
			content += [identifier] + [f"\t{line}" for line in str(entry).split("\n")]
		return "\n".join(content)


class Entities:
	"""Class to handle entities and their dependencies."""

	def __init__(self, symbols: SymbolMap) -> None:
		self.symbols = symbols
		# Map of all available components and their dependencies.
		self.expressions = Components()
		# Map of all available connections.
		self.connections = Connections()

	def processMeta(self, expression: Expression) -> None:
		"""Process a meta expression, a special function from the language."""

		expression.assertTrue(condition=expression.isSymbol, message=f"Meta expressions must have a symbol.")
		if expression.symbol.fqn == "connect":
			self.connections.add(expression.parametersResolved[0].param, expression.parametersResolved[1].param)

			self.add(expression.parametersResolved[0].param, isDep=True)
			self.add(expression.parametersResolved[1].param, isDep=True)

		else:
			expression.error(message="Unsupported meta expression.")

	@cached_property
	def registry(self) -> typing.Dict[str, Expression]:
		"""The registry."""
		registry = OrderedDict()
		for e in self.expressions:
			if EntryType.registry in e.entryType:
				registry[e.expression.fqn] = e.expression
		return registry

	@cached_property
	def platform(self) -> typing.Dict[str, Expression]:
		"""The platform entities."""

		platform = OrderedDict()
		for e in self.expressions:
			if EntryType.platform in e.entryType:
				platform[e.expression.fqn] = e.expression
		return platform

	@cached_property
	def executors(self) -> typing.Dict[str, Expression]:
		"""The executor entities."""

		executors = OrderedDict()
		for e in self.expressions:
			if EntryType.executor in e.entryType:
				executors[e.expression.fqn] = e.expression
		return executors

	@cached_property
	def workloads(self) -> typing.Iterable[ExpressionEntry]:
		"""List all workloads."""
		return [e for e in self.expressions if EntryType.workload in e.entryType]

	@cached_property
	def services(self) -> typing.Iterable[ExpressionEntry]:
		"""List all services."""
		return [e for e in self.expressions if EntryType.service in e.entryType]

	def getWorkloadsByExecutor(self, fqn: str) -> typing.Iterable[Expression]:
		return [entry.expression for entry in self.workloads if fqn in entry.executors]

	def getServicesByExecutor(self, fqn: str) -> typing.Iterable[Expression]:
		return [entry.expression for entry in self.services if fqn in entry.executors]

	def add(self, entity: Entity, isDep: bool = False, executor: typing.Optional[str] = None) -> None:
		"""Add a new entity."""

		entity.assertTrue(condition=isinstance(entity, Expression),
			message="Only expressions can be added to the composition.")
		expression = typing.cast(Expression, entity)

		# If the entity does not have a fqn (hence a namespace), process its dependencies.
		if not expression.isFQN:
			for fqn in expression.dependencies:
				dep = self.symbols.getEntityResolved(fqn=fqn).value
				if isinstance(dep, Expression):
					self.add(dep, isDep=True)
			return

		# Resolve the entity against its namespace.
		resolver = self.symbols.makeResolver(namespace=expression.namespace)
		expression.resolveMemoized(resolver=resolver)

		if expression.isRoleMeta:
			self.processMeta(expression=expression)
		else:
			self.processEntry(expression=expression, isDep=isDep, resolver=resolver, executor=executor)

	def processEntry(self, expression: Expression, isDep: bool, resolver: Resolver,
		executor: typing.Optional[str]) -> None:
		"""Resolve the dependencies for a specific expression."""

		# Find the symbol underlying type.
		underlyingType = expression.getEntityUnderlyingTypeResolved(resolver=resolver)

		# Identify the type of entry.
		if underlyingType.category == Category.method:
			entryType = EntryType.service if isDep else EntryType.workload
		elif underlyingType.category in {Category.component, Category.struct, Category.enum, Category.using}:
			expression.assertTrue(condition=expression.isName,
				message="All expressions from the registry must have a name.")
			entryType = EntryType.registry
		elif underlyingType.category in {Category.builtin}:
			# Ignore all builtins as they are expected to be all the time available.
			return
		else:
			expression.error(
				message=f"Unsupported entry type '{underlyingType.category.value}' within the composition stage.")

		# Check if a platform.
		if expression.namespace == ["platform"]:
			entryType |= EntryType.platform
		# Check if an executor.
		if "bzd.platform.Executor" in underlyingType.getParents():
			expression.assertTrue(condition=underlyingType.category == Category.component,
				message="Executors must be a component.")
			entryType |= EntryType.executor
			executor = expression.fqn

		# If top level, only process a certain type of entry.
		if not (isDep or (EntryType.workload in entryType) or (EntryType.platform in entryType) or
			(EntryType.executor in entryType)):
			return

		# Identify the executor of this entity, it is guarantee by the Expression type that there is always an executor.
		if executor is None:
			executor = self.symbols.getEntityResolved(fqn=expression.executorOr("executor")).assertValue(
				element=expression.element).fqn

		# Register the entry.
		entry = self.expressions.insert(expression=expression, entryType=entryType, executor=executor)
		if entry is None:
			return

		# Look for all dependencies and add the expression only.
		for fqn in expression.dependencies:
			dep = self.symbols.getEntityResolved(fqn=fqn).value
			if isinstance(dep, Expression):
				entry.deps.push(dep)

		# Check if there are pre/post-requisites (init/shutdown functions)
		if underlyingType.isInterface:

			def checkIfInitOrShutdown(interfaceEntity: Entity) -> typing.Optional[DependencyGroup]:
				if interfaceEntity.contracts.has("init"):
					return entry.init
				if interfaceEntity.contracts.has("shutdown"):
					return entry.shutdown
				return None

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

				entry.intra.push(entityCopied)

		# Add implicit dependencies.
		for dependency in entry.deps + entry.intra:
			self.add(dependency, isDep=True, executor=executor)

	def close(self) -> None:
		# Add platform expressions to all executors.
		for expression in self.platform.values():
			for entry in self.expressions.getDependencies(expression):
				entry.executors.update(self.executors.keys())

		resolver = self.symbols.makeResolver()
		self.expressions.close()
		self.connections.resolve(resolver=resolver)

	def __str__(self) -> str:
		content = ["==== Components ====", str(self.expressions)]
		content += ["==== Connections ===="]
		content += [str("\n".join([f"\t- {c}" for c in str(self.connections).split("\n")]))]

		return "\n".join(content)
