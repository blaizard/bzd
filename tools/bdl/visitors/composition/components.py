import typing
import enum
import dataclasses
from collections import OrderedDict

from bzd.utils.result import Result

from tools.bdl.entities.impl.builtin import Builtin
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.impl.fragment.symbol import Symbol
from tools.bdl.entities.impl.types import Category
from tools.bdl.visitors.symbol_map import Resolver, SymbolMap


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


class EntryType(enum.Flag):
	# Items that are part of the registry, should be created before running any asyncs.
	registry = enum.auto()
	# Workload type of asyncs, define the lifetime of the application.
	workload = enum.auto()
	# Service type of asyncs, are stopped when no workloads are running.
	service = enum.auto()


@dataclasses.dataclass(frozen=True)
class Context:
	# The executor for this context.
	executor: str
	# The target for this context.
	target: str

	@property
	def executorWithoutTarget(self) -> str:
		"""Return the executor name without the target name."""

		return self.executor[len(self.target) + 1:]

	def makeResolver(self, symbols: SymbolMap, expression: Expression, **kwargs: typing.Any) -> Resolver:
		"""Create a resolver object, used to find (resolve) symbols."""

		return expression.makeResolver(symbols=symbols, target=self.target, **kwargs)

	def __repr__(self) -> str:
		return f"{self.target}::{self.executor}"


@dataclasses.dataclass
class ExpressionEntry:
	# The expression itself.
	expression: Expression
	# The asynchronous type of this entity.
	entryType: EntryType
	# Dependencies over other entities, first level dependencies.
	deps: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	# Direct dependencies.
	init: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	intra: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)
	shutdown: DependencyGroup = dataclasses.field(default_factory=DependencyGroup)

	@property
	def isRegistry(self) -> bool:
		return EntryType.registry in self.entryType

	@property
	def isWorkload(self) -> bool:
		return EntryType.workload in self.entryType

	@property
	def isService(self) -> bool:
		return EntryType.service in self.entryType

	def __repr__(self) -> str:
		content = [
		    f"type: {str(self.entryType)}",
		    f"expression: {str(self.expression)}",
		    f"deps: {self.deps}",
		    f"init: {self.init}",
		    f"intra: {self.intra}",
		    f"shutdown: {self.shutdown}",
		]
		return "\n".join(content)


class Components:

	def __init__(self) -> None:
		self.map: typing.Dict[str, typing.Dict[Context, ExpressionEntry]] = {}
		# Flag flipped when the data is resolved.
		self.isResolved = False
		# Ordered view of the map, computed only after close.
		self.resolved: typing.Dict[Context, typing.List[ExpressionEntry]] = {}

	@property
	def contexts(self) -> typing.Set[Context]:
		"""Collection of all context available in this instance."""

		assert self.isResolved
		return set(self.resolved.keys())

	@property
	def all(self) -> typing.Dict[Context, typing.List[ExpressionEntry]]:
		"""Collection of all expression entities available in this instance."""

		assert self.isResolved
		return self.resolved

	@staticmethod
	def makeId(expression: Expression) -> str:
		"""Create an identifier from an expression.
        An Identifier is created with the name and the symbol or value.
        """
		expression.assertTrue(
		    condition=expression.isSymbol,
		    message="Only expressions with symbol can be part of the 'Components' list.",
		)

		if expression.isName:
			identifier = expression.fqn
		else:
			identifier = str(expression.symbol)
		return identifier

	def resolve(self) -> None:
		"""Resolve the dependencies to ensure that each entry can be created sequentially
        with only previously created entries as dependencies."""

		assert not self.isResolved

		self.resolved = {}
		contexts = {context for entries in self.map.values() for context in entries.keys()}
		for context in contexts:
			self.resolved[context] = []
			discovered = set()

			def isExpressionDiscovered(expression: Expression) -> bool:
				return self.makeId(expression) in discovered

			def isSatisfied(group: DependencyGroup) -> bool:
				"""Check that the group dependency is statified with the current map."""

				for expression in group:
					if not isExpressionDiscovered(expression):
						return False
				return True

			entries = [(identifier, entries[context]) for identifier, entries in self.map.items() if context in entries]
			while entries:
				remaining: typing.List[typing.Tuple[str, ExpressionEntry]] = []
				for identifier, entry in entries:
					if isSatisfied(entry.deps):
						self.resolved[context].append(entry)
						discovered.add(identifier)
					else:
						remaining.append((identifier, entry))

				if len(entries) == len(remaining):
					entry = entries[0][1]
					dependenciesNotMet = [str(e) for e in entry.deps if not isExpressionDiscovered(e)]
					entry.expression.error(
					    message=f"Not all dependencies of this expression are met: {', '.join(dependenciesNotMet)}")

				entries = remaining

		self.isResolved = True

	def close(self) -> None:
		self.resolve()

	def insert(self, expression: Expression, entryType: EntryType,
	           context: Context) -> typing.Optional[ExpressionEntry]:
		"""Insert a new entry in the map, return the entry if it does not exists or if it can be updated,
        otherwise it returns None."""

		assert not self.isResolved
		identifier = self.makeId(expression)

		if identifier not in self.map:
			self.map[identifier] = {}
		entries = self.map[identifier]

		# Ensure that only a workload can supersede an existing expression with the same id/context.
		if context in entries:

			def isOverwritable(entryTypeFrom: EntryType, entryTypeTo: EntryType) -> bool:
				return (EntryType.service in entryTypeFrom) and (EntryType.workload in entryTypeTo)

			# If it has the same type.
			if entries[context].entryType == entryType:
				return None
			# If it is already overwritten.
			if isOverwritable(entryType, entries[context].entryType):
				return None
			expression.assertTrue(
			    condition=isOverwritable(entries[context].entryType, entryType),
			    message=
			    f"An expression of role '{entryType}' cannot overwrite an expression of role '{entries[context].entryType}'.",
			)

		# Note, it is important to do a copy of the expression, as multiple expression will co-exist with different
		# arguments, for example when used with `target.out`.
		entries[context] = ExpressionEntry(expression=expression.deepCopy(), entryType=entryType)
		return entries[context]

	def fromIdentifier(self, identifier: str, context: Context) -> Result[ExpressionEntry, str]:
		"""Get the ExpressionEntry associated with a specific symbol."""

		if identifier not in self.map:
			return Result.makeError(f"The expression with the identifier '{identifier}' is not registered in the map.")
		if context not in self.map[identifier]:
			return Result.makeError(
			    f"The expression with the identifier '{identifier}' matches but does not have a entry matching '{context}'."
			)
		return Result(self.map[identifier][context])

	def __repr__(self) -> str:
		content = []
		for identifier, entries in self.map.items():
			content += [identifier]
			for context, entry in entries.items():
				content += [f"\t{context}"]
				content += [f"\t\t{line}" for line in str(entry).split("\n")]
		return "\n".join(content)
