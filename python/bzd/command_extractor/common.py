import typing
import shlex
import dataclasses
import re
import enum
import pathlib


@dataclasses.dataclass
class Item:
	"""Base class for any items."""

	original: typing.List[str]
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
	pathOrValue: typing.Union[pathlib.Path, str]

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

	original: typing.List[str] = dataclasses.field(default_factory=list)
	args: typing.List[str] = dataclasses.field(default_factory=list)

	def make(self, Kind: typing.Type[T], *args: typing.Any, **kwargs: typing.Any) -> T:
		assert issubclass(Kind, Item), f"The type '{Kind}' must be a subclass of 'Item'."
		return typing.cast(T, Kind(self.original, *args, **kwargs))


@dataclasses.dataclass
class Processor:
	count: int
	processor: typing.Callable[..., None]


Fallback = typing.Callable[[ItemFactory, str], None]
Schema = typing.Mapping[str, Processor]


class CommandExtractor:

	def __init__(self) -> None:
		self.result: typing.List[Item] = []

	def generateItemString(self, kinds: typing.Mapping[str, enum.Enum]) -> typing.Dict[str, Processor]:
		"""Helper to generate batched schema entries for ItemString."""
		output: typing.Dict[str, Processor] = {}
		for kind, category in kinds.items():
			output[re.escape(kind)] = Processor(
			    1, lambda factory, x, category=category: self.result.append(factory.make(ItemString, category, x)))
		return output

	def generateItem(self, kinds: typing.Mapping[str, enum.Enum]) -> typing.Dict[str, Processor]:
		"""Helper to generate batched schema entries for Item."""
		output: typing.Dict[str, Processor] = {}
		for kind, category in kinds.items():
			output[re.escape(kind)] = Processor(
			    0, lambda factory, category=category: self.result.append(factory.make(Item, category)))
		return output

	def parse(self, cmdString: str, schema: Schema, fallback: Fallback) -> None:

		activeSchema: typing.Optional[Processor] = None
		factory: typing.Optional[ItemFactory] = None
		for arg in shlex.split(cmdString):

			remmainder = arg
			if activeSchema is None:
				isMatch = False
				for keys, entry in schema.items():
					if re.match(r'^' + keys, arg):
						assert not isMatch, f"There are more than one match: {keys}"
						isMatch = True
						activeSchema = entry
						factory = ItemFactory()
						remmainder = re.sub(r'^' + keys + '=?', "", arg)

				if not isMatch:
					fallback(ItemFactory(original=[arg]), arg)

			if activeSchema:
				assert factory, "Must be set with activeSchema."
				if remmainder:
					factory.args.append(remmainder)
				factory.original.append(arg)
				if len(factory.args) == activeSchema.count:
					activeSchema.processor(factory, *factory.args)
					activeSchema = None

	def values(self, exclude: typing.Dict[enum.Enum, typing.Optional[typing.Set[str]]] = {}) -> typing.Iterable[Item]:
		for value in self.result:
			if value.category in exclude:
				matches = exclude[value.category]
				if not matches or value.filter(*matches):
					continue
			yield value
