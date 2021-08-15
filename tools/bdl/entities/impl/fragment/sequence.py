import typing

from collections import defaultdict

if typing.TYPE_CHECKING:
	from tools.bdl.entities.all import Expression, Builtin, Nested, Method, Using, Enum, Namespace, Use, EntityType


class EntitySequence:
	"""
	Defines an entity sequence
	"""

	def __init__(self, sequence: typing.List["EntityType"]) -> None:
		self.list = sequence
		self.processed: typing.Optional[typing.Dict[str, typing.List["EntityType"]]] = None

	def __iter__(self) -> typing.Iterator["EntityType"]:
		for entity in self.list:
			yield entity

	def __getitem__(self, index: int) -> "EntityType":
		return self.list[index]

	def __len__(self) -> int:
		return len(self.list)

	def __repr__(self) -> str:
		return "\n".join([str(entity) for entity in self.list])

	def _filteredList(self, category: str) -> typing.List["EntityType"]:
		if not self.processed:
			self.processed = defaultdict(list)
			for entity in self.list:
				self.processed[entity.category].append(entity)
		return self.processed.get(category, [])

	def _is(self, category: str) -> bool:
		return bool(self._filteredList(category))

	@property
	def isNested(self) -> bool:
		return self._is("nested")

	@property
	def nestedList(self) -> typing.List["Nested"]:
		return typing.cast(typing.List["Nested"], self._filteredList("nested"))

	@property
	def isExpression(self) -> bool:
		return self._is("expression")

	@property
	def expressionList(self) -> typing.List["Expression"]:
		return typing.cast(typing.List["Expression"], self._filteredList("expression"))

	@property
	def isMethod(self) -> bool:
		return self._is("method")

	@property
	def methodList(self) -> typing.List["Method"]:
		return typing.cast(typing.List["Method"], self._filteredList("method"))

	@property
	def isUsing(self) -> bool:
		return self._is("using")

	@property
	def usingList(self) -> typing.List["Using"]:
		return typing.cast(typing.List["Using"], self._filteredList("using"))

	@property
	def isEnum(self) -> bool:
		return self._is("enum")

	@property
	def enumList(self) -> typing.List["Enum"]:
		return typing.cast(typing.List["Enum"], self._filteredList("enum"))

	@property
	def isNamespace(self) -> bool:
		return self._is("namespace")

	@property
	def namespaceList(self) -> typing.List["Namespace"]:
		return typing.cast(typing.List["Namespace"], self._filteredList("namespace"))

	@property
	def isUse(self) -> bool:
		return self._is("use")

	@property
	def useList(self) -> typing.List["Use"]:
		return typing.cast(typing.List["Use"], self._filteredList("use"))
