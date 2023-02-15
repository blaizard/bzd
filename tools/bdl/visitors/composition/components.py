import typing
import enum
import dataclasses
from collections import OrderedDict

from bzd.utils.result import Result

from tools.bdl.entities.impl.builtin import Builtin
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.impl.fragment.symbol import Symbol
from tools.bdl.entities.impl.types import Category
from tools.bdl.visitors.symbol_map import Resolver


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
		expression.assertTrue(condition=expression.isSymbol,
		                      message="Only expressions with symbol can be part of the 'Components' list.")

		if expression.isName:
			identifier = expression.fqn
		else:
			identifier = expression.symbol.fqn
		return identifier

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
		self.map = OrderedDict()

		while entries:

			remaining: typing.List[typing.Tuple[str, ExpressionEntry]] = []
			for identifier, entry in entries:
				if isSatisfied(entry.deps):
					self.map[identifier] = entry
				else:
					remaining.append((identifier, entry))

			if len(entries) == len(remaining):
				entries[0][1].expression.error(message="The dependencies of this expression are not met.")

			entries = remaining

	def close(self) -> None:
		self.resolve()

	def __contains__(self, expression: Expression) -> bool:
		"""Check if an entry already exists in the map."""

		return self.makeId(expression) in self.map

	def __getitem__(self, expression: Expression) -> Result[ExpressionEntry, str]:
		"""Get a specific item."""

		identifier = self.makeId(expression)
		return self.fromIdentifier(identifier=identifier)

	def __iter__(self) -> typing.Iterator[ExpressionEntry]:
		for entry in self.map.values():
			yield entry

	def insert(self, expression: Expression, entryType: EntryType,
	           executor: typing.Optional[str]) -> typing.Optional[ExpressionEntry]:
		"""Insert a new entry in the map, return the entry if it does not exists or if it can be updated,
		otherwise it returns None."""

		identifier = self.makeId(expression)

		# Ensure that only a workload can superseed an existing expression with the same id.
		if identifier in self.map:

			def isOverwritable(entryTypeFrom: EntryType, entryTypeTo: EntryType) -> bool:
				return (EntryType.service in entryTypeFrom) and (EntryType.workload in entryTypeTo)

			# If this is the exact same expression.
			if self.map[identifier].entryType == entryType:
				expression.assertTrue(
				    condition=(self.map[identifier].expression == expression),
				    message=f"This expression already exits and cannot be redeclared as the same role: '{entryType}'.")
				return None
			if isOverwritable(entryType, self.map[identifier].entryType):
				return None
			expression.assertTrue(
			    condition=isOverwritable(self.map[identifier].entryType, entryType),
			    message=
			    f"An expression of role '{entryType}' cannot overwrite an expression of role '{self.map[identifier].entryType}'."
			)

		executors = set() if executor is None else {executor}
		self.map[identifier] = ExpressionEntry(expression=expression, entryType=entryType, executors=executors)
		return self.map[identifier]

	def fromSymbol(self, symbol: Symbol) -> Result[ExpressionEntry, str]:
		"""Get the ExpressionEntry associated with a specific symbol."""

		identifier = symbol.this if symbol.isThis else symbol.kind
		return self.fromIdentifier(identifier=identifier)

	def fromIdentifier(self, identifier: str) -> Result[ExpressionEntry, str]:
		"""Get the ExpressionEntry associated with a specific symbol."""

		if identifier not in self.map:
			return Result.makeError(f"The expression with the identifier '{identifier}' is not registered in the map.")
		return Result(self.map[identifier])

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
