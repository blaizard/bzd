import typing
from collections import defaultdict

from bdl.entities.impl.types import Category

if typing.TYPE_CHECKING:
	from bdl.entities.all import (
		EntityType,
		Enum,
		Expression,
		Method,
		Namespace,
		Nested,
		Use,
		Using,
	)


class EntitySequence:
	"""
	Defines an entity sequence
	"""

	def __init__(self, sequence: list["EntityType"]) -> None:
		self.sequence_ = sequence
		self.processed: dict[Category, list[EntityType]] | None = None

	@property
	def sequence(self) -> list["EntityType"]:
		"""
		Used for customization.
		"""
		return self.sequence_

	def __iter__(self) -> typing.Iterator["EntityType"]:
		yield from self.sequence

	def __len__(self) -> int:
		return len(self.sequence)

	def __repr__(self) -> str:
		return "\n".join([str(entity.element) for entity in self.sequence])

	def _filteredList(self, *categories: Category) -> list["EntityType"]:
		if not self.processed:
			self.processed = defaultdict(list)
			for entity in self.sequence:
				self.processed[entity.category].append(entity)
		output = []
		for category in categories:
			output += self.processed.get(category, [])
		return output

	def _is(self, *categories: Category) -> bool:
		return bool(self._filteredList(*categories))

	@property
	def isNested(self) -> bool:
		return self._is(
			Category.struct,
			Category.interface,
			Category.component,
			Category.composition,
		)

	@property
	def nestedList(self) -> list["Nested"]:
		return typing.cast(
			list["Nested"],
			self._filteredList(
				Category.struct,
				Category.interface,
				Category.component,
				Category.composition,
			),
		)

	@property
	def isExpression(self) -> bool:
		return self._is(Category.expression)

	@property
	def expressionList(self) -> list["Expression"]:
		return typing.cast(list["Expression"], self._filteredList(Category.expression))

	@property
	def isMethod(self) -> bool:
		return self._is(Category.method)

	@property
	def methodList(self) -> list["Method"]:
		return typing.cast(list["Method"], self._filteredList(Category.method))

	@property
	def isUsing(self) -> bool:
		return self._is(Category.using)

	@property
	def usingList(self) -> list["Using"]:
		return typing.cast(list["Using"], self._filteredList(Category.using))

	@property
	def isEnum(self) -> bool:
		return self._is(Category.enum)

	@property
	def enumList(self) -> list["Enum"]:
		return typing.cast(list["Enum"], self._filteredList(Category.enum))

	@property
	def isNamespace(self) -> bool:
		return self._is(Category.namespace)

	@property
	def namespace(self) -> "Namespace":
		return typing.cast("Namespace", self._filteredList(Category.namespace)[0])

	@property
	def namespaceList(self) -> list["Namespace"]:
		return typing.cast(list["Namespace"], self._filteredList(Category.namespace))

	@property
	def isUse(self) -> bool:
		return self._is(Category.use)

	@property
	def useList(self) -> list["Use"]:
		return typing.cast(list["Use"], self._filteredList(Category.use))
