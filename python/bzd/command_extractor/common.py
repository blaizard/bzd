import dataclasses
import enum
import pathlib
import re
import shlex
import typing


@dataclasses.dataclass
class Item:
	"""Base class for any items."""

	original: list[str]
	category: enum.Enum

	def filter(self, *args: typing.Any) -> bool:
		return True

	def __str__(self) -> str:
		return " ".join(self.original)


@dataclasses.dataclass
class ItemString(Item):
	"""Base item that contains a value string."""

	# A value associated with this item
	value: str

	def filter(self, *values: str) -> bool:
		return any(re.match(regexpr, self.value) for regexpr in values)


@dataclasses.dataclass
class ItemPath(Item):
	"""Base item that contains a path."""

	# The actual path
	path: pathlib.Path


@dataclasses.dataclass
class ItemPathOrString(Item):
	"""Base item that contains a path or a string as value."""

	# The actual path or a name
	pathOrValue: pathlib.Path | str

	@property
	def hasPath(self) -> bool:
		return isinstance(self.pathOrValue, pathlib.Path)

	@property
	def hasValue(self) -> bool:
		return isinstance(self.pathOrValue, str)


T = typing.TypeVar("T")


@dataclasses.dataclass
class ItemFactory:
	"""Factory to create an item."""

	original: list[str] = dataclasses.field(default_factory=list)
	args: list[str] = dataclasses.field(default_factory=list)

	def make(self, Kind: type[T], *args: typing.Any, **kwargs: typing.Any) -> T:
		assert issubclass(Kind, Item), f"The type '{Kind}' must be a subclass of 'Item'."
		return Kind(self.original, *args, **kwargs)


@dataclasses.dataclass
class Processor:
	count: int
	processor: typing.Callable[..., None]


Fallback = typing.Callable[[ItemFactory, str], None]
Schema = typing.Mapping[str, Processor]


class CommandExtractor:
	def __init__(self) -> None:
		self.result: list[Item] = []

	def generateItemString(self, kinds: typing.Mapping[str, enum.Enum]) -> dict[str, Processor]:
		"""Helper to generate batched schema entries for ItemString."""
		output: dict[str, Processor] = {}
		for kind, category in kinds.items():
			output[re.escape(kind)] = Processor(
				1,
				lambda factory, x, category=category: self.result.append(factory.make(ItemString, category, x)),
			)
		return output

	def generateItem(self, kinds: typing.Mapping[str, enum.Enum]) -> dict[str, Processor]:
		"""Helper to generate batched schema entries for Item."""
		output: dict[str, Processor] = {}
		for kind, category in kinds.items():
			output[re.escape(kind)] = Processor(
				0,
				lambda factory, category=category: self.result.append(factory.make(Item, category)),
			)
		return output

	def parse(self, cmdString: str, schema: list[Schema], fallback: Fallback) -> None:
		activeSchema: Processor | None = None
		factory: ItemFactory | None = None
		matchArgs: set[str] = set()

		# Process schemas by order of priority.
		for schemaIndex, schemaDict in enumerate(schema):
			isLastSchema = schemaIndex == (len(schema) - 1)

			for arg in shlex.split(cmdString):
				remmainder = arg
				if activeSchema is None:
					isMatch = False
					for keys, entry in schemaDict.items():
						if re.match(r"^" + keys, arg):
							assert not isMatch, f"There are more than one match: {keys}"
							isMatch = True
							activeSchema = entry
							factory = ItemFactory()
							remmainder = re.sub(r"^" + keys + "=?", "", arg)

					if isMatch:
						matchArgs.add(arg)
					elif isLastSchema and arg not in matchArgs:
						fallback(ItemFactory(original=[arg]), arg)

				if activeSchema:
					assert factory, "Must be set with activeSchema."
					if remmainder:
						factory.args.append(remmainder)
					factory.original.append(arg)
					if len(factory.args) == activeSchema.count:
						activeSchema.processor(factory, *factory.args)
						activeSchema = None

	def values(self, exclude: dict[enum.Enum, set[str] | None] | None = None) -> typing.Iterable[Item]:
		if exclude is None:
			exclude = {}
		for value in self.result:
			if value.category in exclude:
				matches = exclude[value.category]
				if not matches or value.filter(*matches):
					continue
			yield value
