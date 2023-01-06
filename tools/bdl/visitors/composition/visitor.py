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
		self.entites = Entities(symbols=self.symbols)
		self.registry: typing.Dict[str, Expression] = {}
		self.platform: typing.Dict[str, Expression] = {}
		# Unique identifiers
		self.uids: typing.Dict[str, int] = {}
		# All top level expressions
		self.all: typing.Dict[str, Expression] = {}
		# All executors.
		self.executors: typing.Dict[str, Expression] = {}
		# All composition per executors.
		self.composition: typing.Dict[str, typing.Dict[Entity, AsyncType]] = {}
		# Connection map
		self.connections: typing.Dict[str, str]

	def visit(self, bdl: Object) -> "Composition":

		# Build a master symbol list
		self.symbols.update(bdl.symbols)
		return self

	def addExecutor(self, entity: Expression) -> None:
		"""Add a new executor from the given expression."""

		entity.assertTrue(entity.executor in self.registry, f"The executor '{entity.executor}' is not declared.")
		self.executors[entity.executor] = self.registry[entity.executor]

	def entity(self, fqn: str) -> Entity:
		"""Get the entity refered to the given fqn."""
		return self.symbols.getEntityResolved(fqn=fqn).value

	def generateUids(self) -> None:
		"""Generate the unique identifiers of the types."""

		counter = 1
		for fqn, entity in sorted(self.symbols.items(groups={Group.globalGroup})):
			if entity.category in [Category.expression, Category.struct, Category.enum]:
				self.uids[fqn] = counter
				counter += 1

	def process(self) -> None:

		self.generateUids()

		self.all = {
			fqn: entity
			for fqn, entity in self.symbols.items(groups={Group.globalComposition}) if isinstance(entity, Expression)
		}
		self.registry = {fqn: entity for fqn, entity in self.all.items() if entity.isName}
		self.executors = {}

		self.entites.update(self.all.values())

		# Applications are all intra expressions that are instanciated at top level
		self.composition = {}
		for fqn, entity in self.all.items():
			if entity.isName or entity.isRoleMeta:
				continue
			entity.assertTrue(entity.executor in self.registry, f"The executor '{entity.executor}' is not declared.")
			self.composition.setdefault(entity.executor, dict())[entity] = AsyncType.workload
			self.addExecutor(entity)

		# Handle platform elements
		for fqn, entity in self.all.items():
			if fqn.startswith("platform"):
				self.platform[fqn] = entity

		# Services are all intra expressions that are deps from all tasks, associated executor and infra.
		commonServices = self.entites.findAllIntra(self.platform.values())  # type: ignore
		for fqn, executorComposition in self.composition.items():
			services = commonServices + self.entites.findAllIntra(
				[self.registry[fqn]]) + self.entites.findAllIntra([*executorComposition.keys()])
			executorComposition.update({entity: AsyncType.service for entity in services})

	def __str__(self) -> str:
		"""Print a human readable view of this instance."""

		def addContent(content: typing.List[str], title: str, contentList: typing.Iterable[typing.Any]) -> None:
			content.append(f"==== {title} ====")
			content += [f"\t- {item}" for item in contentList]

		content: typing.List[str] = []
		addContent(content, "Includes", self.includes)
		addContent(content, "Symbols", str(self.symbols).split("\n"))
		addContent(content, "Unique Identifiers", [f"{k}: {v}" for k, v in self.uids.items()])
		addContent(content, "Entites", str(self.entites).split("\n"))
		addContent(content, "Registry", self.registry.values())
		addContent(content, "Platform", self.platform.keys())
		addContent(content, "Executors", self.executors.keys())
		for executor, items in self.composition.items():
			addContent(content, f"Composition '({executor})'", [f"{k}: {v}" for k, v in items.items()])

		return "\n".join(content)
