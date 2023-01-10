import typing
import dataclasses
import enum
from functools import cached_property

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


class EntryType(enum.Enum):
	# Items that are part of the registry, should be created before running any asyncs.
	registry = "registry"
	# Worload type of asyncs, define the lifetime of the application.
	workload = "workload"
	# Service type of asyncs, are stopped when no workloads are running.
	service = "service"


@dataclasses.dataclass
class ExpressionEntry:
	# The expression itself.
	expression: Expression
	# The asynchronous type of this entity.
	entryType: EntryType
	# The executor associated with this entry.
	executor: typing.Optional[Expression] = None
	# Dependencies over other entities, first level dependencies.
	deps: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	# Direct dependencies.
	init: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	intra: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	shutdown: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)

	def __repr__(self) -> str:
		content = [
			f"type: {str(self.entryType)}", f"expression: {str(self.expression)}", f"executor: {str(self.executor)}",
			f"deps: {self.deps}", f"init: {self.init}", f"intra: {self.intra}", f"shutdown: {self.shutdown}"
		]
		return "\n".join(content)


class Components:

	def __init__(self) -> None:
		self.map: typing.Dict[str, ExpressionEntry] = {}

	@staticmethod
	def makeId(expression: Expression) -> str:
		"""Create an identifier from an expression.
		An Identifier is created with the name and the symbol or value.
		"""

		result = f"{expression.name}|" if expression.isName else "|"
		result += expression.value if expression.isValue else str(expression.symbol.fqn)
		return result

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

	def insert(self, expression: Expression, entryType: EntryType) -> ExpressionEntry:
		"""Insert a new entry in the map, return the entry if it does not exists or if it can be updated,
		otherwise it returns None."""

		identifier = self.makeId(expression)

		# Ensure that only a workload can superseed an existing expression with the same id.
		if identifier in self.map:

			def isOverwritable(entryTypeFrom: EntryType, entryTypeTo: EntryType) -> bool:
				return (entryTypeFrom == EntryType.service) and (entryTypeTo == EntryType.workload)

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

		self.map[identifier] = ExpressionEntry(expression=expression, entryType=entryType)
		return self.map[identifier]

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

		else:
			expression.error(message="Unsupported meta expression.")

	def resolveDependencies(self, entities: typing.List[Expression]) -> typing.List[Expression]:
		"""From entities that are part of self.expressions, resolve the dependencies to ensure
		that each entity can be create sequentially with only previously created entites as dependencies."""

		resolved: typing.List[Expression] = []

		while entities:

			remaining: typing.List[Expression] = []
			for entity in entities:
				if self.expressions[entity].value.deps.isSubset(resolved):
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
		return self.resolveDependencies([e.expression for e in self.expressions if e.entryType == EntryType.registry])

	@cached_property
	def workloads(self) -> typing.Iterable[Expression]:
		"""List all workloads."""
		return [e.expression for e in self.expressions if e.entryType == EntryType.workload]

	@cached_property
	def services(self) -> typing.Iterable[Expression]:
		"""List all services."""
		return [e.expression for e in self.expressions if e.entryType == EntryType.service]

	@property
	def platform(self) -> typing.Iterable[typing.Tuple[str, Expression]]:
		"""The platform entities."""
		for entry in self.expressions:
			if self.isPlatform(entry.expression):
				yield entry.expression.fqn, entry.expression

	@staticmethod
	def isPlatform(expression: Expression) -> bool:
		return expression.isName and expression.fqn.startswith("platform")

	def add(self, entity: Entity, isDep: bool = False) -> None:
		"""Add a new entity."""

		entity.assertTrue(condition=isinstance(entity, Expression),
			message="Only expressions can be added to the composition.")
		expression = typing.cast(Expression, entity)

		# Resolve the entity against its namespace.
		resolver = self.symbols.makeResolver(namespace=expression.namespace)
		expression.resolveMemoized(resolver=resolver)

		if expression.isRoleMeta:
			self.processMeta(expression=expression)
		else:
			self.processEntry(expression=expression, isDep=isDep, resolver=resolver)

	def processEntry(self, expression: Expression, isDep: bool, resolver: Resolver) -> None:
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

		# If not dep, only process workload and platform entries.
		if not (isDep or entryType == EntryType.workload or self.isPlatform(expression)):
			return

		# Register the entry.
		entry = self.expressions.insert(expression=expression, entryType=entryType)
		if entry is None:
			return

		# Set the executor, it is guarantee by the Expressiontype that there is always an executor.
		entry.executor = self.symbols.getEntityResolved(fqn=expression.executor).assertValue(element=expression.element)
		self.add(entry.executor, isDep=True)

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
			self.add(dependency, isDep=True)

	def __str__(self) -> str:
		content = ["==== Components ====", str(self.expressions)]
		content += ["==== Connections ===="]
		content += [str("\n".join([f"\t- {c}" for c in str(self.connections).split("\n")]))]

		return "\n".join(content)
