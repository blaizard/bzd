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


class CompositionView:
	"""Create a composition view, this is the interface exposed to the composition template."""

	def __init__(self, composition: "Composition", target: typing.Optional[str]) -> None:

		self.composition = composition
		self.target = target

		if target:
			assert target in self.composition.targets, f"Using an unknown target '{target}', known targets are: {self.composition.targets}"
			self.excluding = self.composition.targets.difference({target})
		else:
			self.excluding = set()

	def entity(self, fqn: str) -> Entity:
		"""Get the entity refered to the given fqn."""

		return self.symbols.getEntityResolved(fqn=fqn).value

	def isValidTarget(self, target: str) -> bool:
		return all((target != excluding) for excluding in self.excluding)

	def isValidFQN_(self, fqn: str) -> bool:
		for target in self.excluding:
			if fqn.startswith(target + "."):
				return False
		return True

	def stripTargetFromFQN(self, fqn: str) -> str:
		if self.target and fqn.startswith(self.target + "."):
			return fqn[(len(self.target) + 1):]
		return fqn

	def filterDictOfFQN_(self, dictOfFQN: typing.Dict[str, typing.Any]) -> typing.Dict[str, typing.Any]:
		return {fqn: item for fqn, item in dictOfFQN.items() if self.isValidFQN_(fqn)}

	@property
	def registry(self) -> typing.Dict[str, Expression]:
		return self.filterDictOfFQN_(self.composition.registry)

	@property
	def registryByExecutor(self) -> typing.Dict[str, typing.Dict[str, Entity]]:
		return {fqn: self.filterDictOfFQN_(item) for fqn, item in self.composition.registryByExecutor.items()}

	@property
	def platform(self) -> typing.Dict[str, Expression]:
		return self.filterDictOfFQN_(self.composition.platform)

	@property
	def executors(self) -> typing.Dict[str, Expression]:
		return self.filterDictOfFQN_(self.composition.executors)

	@property
	def registryConnections(self) -> typing.Dict[str, Expression]:
		return self.filterDictOfFQN_(self.composition.registryConnections)

	@property
	def connections(self) -> typing.Dict[str, typing.Any]:
		return self.filterDictOfFQN_(self.composition.connections)

	@property
	def asyncs(self) -> typing.Dict[str, typing.Dict[Entity, AsyncType]]:
		output: typing.Dict[str, typing.Dict[Entity, AsyncType]] = {}
		for fqn, dictOfEntities in self.composition.asyncs.items():
			if self.isValidFQN_(fqn):
				output[fqn] = {}
				for entity, data in dictOfEntities.items():
					if self.isValidFQN_(entity.fqn):
						output[fqn][entity] = data
		return output

	@property
	def entities(self) -> Entities:
		return self.composition.entities

	@property
	def symbols(self) -> SymbolMap:
		return self.composition.symbols

	@property
	def uids(self) -> typing.Dict[str, int]:
		return self.composition.uids


class Composition:

	def __init__(self, targets: typing.Optional[typing.Set[str]] = None) -> None:

		# All targets available, an empty set if there are no target for this composition.
		self.targets = targets if targets else set()
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
		"""Add a specific BDL object to the composition.

		Args:
			bdl: The object to visit.
		"""

		# Build a master symbol list
		self.symbols.update(bdl.symbols)
		return self

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
			self.registryByExecutor[executorFQN] = self.entities.getRegistryByExecutor(executorFQN)  # type: ignore
			for entity in self.entities.getWorkloadsByExecutor(executorFQN):
				self.asyncs[executorFQN][entity] = AsyncType.workload
			for entity in self.entities.getServicesByExecutor(executorFQN):
				self.asyncs[executorFQN][entity] = AsyncType.service
			self.connections[executorFQN] = [*self.entities.getConnectionsByExecutor(executorFQN)]

	def view(self, target: typing.Optional[str] = None) -> CompositionView:
		"""Get a composition view for a specific target."""

		return CompositionView(self, target=target)

	def __str__(self) -> str:
		"""Print a human readable view of this instance."""

		def addContent(content: typing.List[str], title: str, contentList: typing.Iterable[typing.Any]) -> None:
			content.append(f"==== {title} ====")
			content += [f"\t- {item}" for item in contentList]

		content: typing.List[str] = []
		addContent(content, "Symbols", str(self.symbols).split("\n"))
		addContent(content, "Unique Identifiers", [f"{k}: {v}" for k, v in self.uids.items()])
		addContent(content, "Entities", str(self.entities).split("\n"))
		addContent(content, "Executors", self.executors.keys())
		for executor, items in self.asyncs.items():
			addContent(content, f"Composition '({executor})'", [f"{k}: {v}" for k, v in items.items()])

		return "\n".join(content)
