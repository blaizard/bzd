import typing

from collections import defaultdict
from tools.bdl.entities.impl.types import Category

if typing.TYPE_CHECKING:
	from tools.bdl.entities.all import Expression, Extern, Builtin, Nested, Method, Using, Enum, Namespace, Use, EntityType


class EntitySequence:
	"""
	Defines an entity sequence
	"""

	def __init__(self, sequence: typing.List["EntityType"]) -> None:
		self.sequence_ = sequence
		self.processed: typing.Optional[typing.Dict[Category, typing.List["EntityType"]]] = None

	@property
	def sequence(self) -> typing.List["EntityType"]:
		"""
		Used for customization.
		"""
		return self.sequence_

	def __iter__(self) -> typing.Iterator["EntityType"]:
		for entity in self.sequence:
			yield entity

	def __len__(self) -> int:
		return len(self.sequence)

	def __repr__(self) -> str:
		return "\n".join([str(entity.element) for entity in self.sequence])

	def _filteredList(self, *categories: Category) -> typing.List["EntityType"]:
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
		return self._is(Category.struct, Category.interface, Category.component, Category.composition)

	@property
	def nestedList(self) -> typing.List["Nested"]:
		return typing.cast(
			typing.List["Nested"],
			self._filteredList(Category.struct, Category.interface, Category.component, Category.composition))

	@property
	def isExpression(self) -> bool:
		return self._is(Category.expression)

	@property
	def expressionList(self) -> typing.List["Expression"]:
		return typing.cast(typing.List["Expression"], self._filteredList(Category.expression))

	@property
	def isMethod(self) -> bool:
		return self._is(Category.method)

	@property
	def methodList(self) -> typing.List["Method"]:
		return typing.cast(typing.List["Method"], self._filteredList(Category.method))

	@property
	def isUsing(self) -> bool:
		return self._is(Category.using)

	@property
	def usingList(self) -> typing.List["Using"]:
		return typing.cast(typing.List["Using"], self._filteredList(Category.using))

	@property
	def isEnum(self) -> bool:
		return self._is(Category.enum)

	@property
	def enumList(self) -> typing.List["Enum"]:
		return typing.cast(typing.List["Enum"], self._filteredList(Category.enum))

	@property
	def isNamespace(self) -> bool:
		return self._is(Category.namespace)

	@property
	def namespace(self) -> "Namespace":
		return typing.cast("Namespace", self._filteredList(Category.namespace)[0])

	@property
	def namespaceList(self) -> typing.List["Namespace"]:
		return typing.cast(typing.List["Namespace"], self._filteredList(Category.namespace))

	@property
	def isUse(self) -> bool:
		return self._is(Category.use)

	@property
	def useList(self) -> typing.List["Use"]:
		return typing.cast(typing.List["Use"], self._filteredList(Category.use))
