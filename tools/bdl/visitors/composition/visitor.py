import typing
import pathlib
import dataclasses
import enum

from bzd.parser.error import Error

from tools.bdl.visitor import Group
from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.visitors.composition.entities import Entities
from tools.bdl.visitors.composition.components import Context, ExpressionEntry
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

	def __init__(self, composition: "Composition", target: str) -> None:
		self.composition = composition
		self.target = target
		self.contexts = {context for context in self.composition.contexts if context.target == target}

	@property
	def registry(self) -> typing.Dict[Context, typing.Dict[str, ExpressionEntry]]:
		return {context: data for context, data in self.composition.registry.items() if context in self.contexts}

	@property
	def services(self) -> typing.Dict[Context, typing.List[ExpressionEntry]]:
		return {context: data for context, data in self.composition.services.items() if context in self.contexts}

	@property
	def workloads(self) -> typing.Dict[Context, typing.List[ExpressionEntry]]:
		return {context: data for context, data in self.composition.workloads.items() if context in self.contexts}

	@property
	def ios(self) -> typing.Dict[Context, typing.List[typing.Dict[str, typing.Any]]]:
		return {context: data for context, data in self.composition.ios.items() if context in self.contexts}

	@property
	def iosRegistry(self) -> typing.Dict[str, typing.Dict[str, typing.Any]]:
		return self.composition.iosRegistry

	def isValidTarget(self, target: str) -> bool:
		return target == self.target

	def entity(self, fqn: str) -> Entity:
		"""Get the entity referred to the given fqn."""

		return self.symbols.getEntity(fqn=fqn).value

	@property
	def symbols(self) -> SymbolMap:
		return self.composition.symbols

	@property
	def uids(self) -> typing.Dict[str, int]:
		return self.composition.uids

	def __str__(self) -> str:
		return f"==== Target ====\n\t" + self.target + "\n" + str(self.composition)


class Composition:

	def __init__(self, targets: typing.Set[str]) -> None:
		# All targets available, an empty set if there are no target for this composition.
		self.targets = targets
		self.symbols = SymbolMap()
		self.entities = Entities(symbols=self.symbols, targets=self.targets)
		# Unique identifiers
		self.uids: typing.Dict[str, int] = {}

	@property
	def contexts(self) -> typing.Set[Context]:
		return self.entities.contexts

	@property
	def registry(self) -> typing.Dict[Context, typing.Dict[str, ExpressionEntry]]:
		return self.entities.registry

	@property
	def services(self) -> typing.Dict[Context, typing.List[ExpressionEntry]]:
		return self.entities.services

	@property
	def workloads(self) -> typing.Dict[Context, typing.List[ExpressionEntry]]:
		return self.entities.workloads

	@property
	def ios(self) -> typing.Dict[Context, typing.List[typing.Dict[str, typing.Any]]]:
		return self.entities.ios

	@property
	def iosRegistry(self) -> typing.Dict[str, typing.Dict[str, typing.Any]]:
		return self.entities.iosRegistry

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

		self.entities.process(compositionEntities)

	def view(self, target: str) -> CompositionView:
		"""Get a composition view for a specific target."""

		return CompositionView(self, target=target)

	def __str__(self) -> str:
		"""Print a human readable view of this instance."""

		def addContent(
		    content: typing.List[str],
		    title: str,
		    contentList: typing.Iterable[typing.Any],
		) -> None:
			content.append(f"==== {title} ====")
			content += [f"\t- {item}" for item in contentList]

		content: typing.List[str] = []
		addContent(content, "Symbols", str(self.symbols).split("\n"))
		addContent(content, "Unique Identifiers", [f"{k}: {v}" for k, v in self.uids.items()])
		addContent(content, "Contexts", self.contexts)
		for context in self.contexts:
			addContent(
			    content,
			    f"Registry: {context}",
			    [f"{k}: {v.expression}" for k, v in self.registry.get(context, {}).items()],
			)
			addContent(
			    content,
			    f"Services: {context}",
			    [v.expression for v in self.services.get(context, [])],
			)
			addContent(
			    content,
			    f"Workloads: {context}",
			    [v.expression for v in self.workloads.get(context, [])],
			)
			addContent(
			    content,
			    f"IOs: {context}",
			    [v["identifier"] for v in self.ios.get(context, [])],
			)

		return "\n".join(content)
