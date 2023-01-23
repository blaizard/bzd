import typing
import pathlib
import dataclasses
import enum

from bzd.parser.error import Error

from tools.bdl.visitor import Group
from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.visitors.composition.entities import Entities
from tools.bdl.object import Object
from tools.bdl.entities.impl.entity import Entity
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.impl.method import Method
from tools.bdl.entities.impl.nested import Nested
from tools.bdl.entities.impl.fragment.fqn import FQN
from tools.bdl.entities.impl.types import Category


class AsyncType(enum.Enum):
	workload = "workload"
	service = "service"


class Composition:

	def __init__(self, includes: typing.Optional[typing.List[pathlib.Path]] = None) -> None:

		self.includes = [] if includes is None else includes
		self.symbols = SymbolMap()
		self.entities = Entities(symbols=self.symbols)
		# Unique identifiers
		self.uids: typing.Dict[str, int] = {}
		# All top level expressions
		self.all: typing.Dict[str, Expression] = {}
		# All asyncs per executors.
		self.asyncs: typing.Dict[str, typing.Dict[Entity, AsyncType]] = {}
		# All connections.
		self.connections: typing.Dict[str, typing.Any] = {}
		self.registryByExecutor: typing.Dict[str, typing.Dict[str, Entity]] = {}

	@property
	def registry(self) -> typing.Dict[str, Expression]:
		return self.entities.registry

	@property
	def platform(self) -> typing.Dict[str, Expression]:
		return self.entities.platform

	@property
	def executors(self) -> typing.Dict[str, Expression]:
		return self.entities.executors

	@property
	def registryConnections(self) -> typing.Dict[str, typing.Dict[str, typing.Any]]:
		return self.entities.registryConnections

	def visit(self, bdl: Object) -> "Composition":

		# Build a master symbol list
		self.symbols.update(bdl.symbols)
		return self

	def entity(self, fqn: str) -> Entity:
		"""Get the entity refered to the given fqn."""
		return self.symbols.getEntityResolved(fqn=fqn).value

	def generateUids(self) -> None:
		"""Generate the unique identifiers of the types."""

		counter = 1
		for fqn, entity in sorted(self.symbols.items(groups={Group.interface})):
			if entity.category in {Category.expression, Category.struct, Category.enum}:
				self.uids[fqn] = counter
				counter += 1

	def process(self) -> None:

		self.generateUids()

		compositionEntities = [
			entity for fqn, entity in self.symbols.items(groups={Group.composition | Group.topLevel})
			if isinstance(entity, Expression)
		]

		for entity in compositionEntities:
			self.entities.add(entity)
		self.entities.close()

		# Applications are all intra expressions that are instanciated at top level
		for executorFQN in self.entities.executors:
			self.asyncs[executorFQN] = {}
			self.registryByExecutor[executorFQN] = self.entities.getRegistryByExecutor(executorFQN) # type: ignore
			for entity in self.entities.getWorkloadsByExecutor(executorFQN):
				self.asyncs[executorFQN][entity] = AsyncType.workload
			for entity in self.entities.getServicesByExecutor(executorFQN):
				self.asyncs[executorFQN][entity] = AsyncType.service
			self.connections[executorFQN] = [*self.entities.getConnectionsByExecutor(executorFQN)]

	def __str__(self) -> str:
		"""Print a human readable view of this instance."""

		def addContent(content: typing.List[str], title: str, contentList: typing.Iterable[typing.Any]) -> None:
			content.append(f"==== {title} ====")
			content += [f"\t- {item}" for item in contentList]

		content: typing.List[str] = []
		addContent(content, "Includes", self.includes)
		addContent(content, "Symbols", str(self.symbols).split("\n"))
		addContent(content, "Unique Identifiers", [f"{k}: {v}" for k, v in self.uids.items()])
		addContent(content, "Entities", str(self.entities).split("\n"))
		addContent(content, "Executors", self.executors.keys())
		for executor, items in self.asyncs.items():
			addContent(content, f"Composition '({executor})'", [f"{k}: {v}" for k, v in items.items()])

		return "\n".join(content)
