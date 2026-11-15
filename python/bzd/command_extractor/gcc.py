import enum
import pathlib
import re
import typing

from bzd.command_extractor.common import (
	CommandExtractor,
	Item,
	ItemFactory,
	ItemPath,
	ItemPathOrString,
	ItemString,
	Processor,
)

_defaultItemFactory = ItemFactory()


class Categories(enum.Enum):
	includeSearchPath = enum.auto()
	librarySearchPath = enum.auto()
	outputPath = enum.auto()
	library = enum.auto()
	linkerScript = enum.auto()
	undefine = enum.auto()
	define = enum.auto()
	warning = enum.auto()
	flag = enum.auto()
	mode = enum.auto()
	optimisation = enum.auto()
	debug = enum.auto()
	standard = enum.auto()
	compileOnly = enum.auto()
	unhandled = enum.auto()


Result = typing.Sequence[Item]


class ItemLibrary(ItemPathOrString):
	@property
	def name(self) -> str:
		"""Get the library name."""
		return re.sub(r"lib(.*)\.a", r"\1", pathlib.Path(self.pathOrValue).name)


class CommandExtractorGcc(CommandExtractor):
	def __init__(
		self,
		cwd: pathlib.Path,
		includeSearchPaths: list[pathlib.Path] | None = None,
		libSearchPaths: list[pathlib.Path] | None = None,
	) -> None:
		super().__init__()
		self.cwd = cwd
		if includeSearchPaths is None:
			includeSearchPaths = []
		if libSearchPaths is None:
			libSearchPaths = []
		for path in [cwd] + includeSearchPaths:
			self.addSearchPath(Categories.includeSearchPath, path)
		for path in [cwd] + libSearchPaths:
			self.addSearchPath(Categories.librarySearchPath, path)

	def addLibrary(self, name: str, factory: ItemFactory = _defaultItemFactory) -> None:
		"""Add a new library to the list."""

		fileName = name if re.match(r"lib.*\.a", pathlib.Path(name).name) else f"lib{name}.a"
		for path in self.librarySearchPaths:
			fullPath = path / fileName
			if fullPath.is_file():
				self.result.append(factory.make(ItemLibrary, Categories.library, fullPath))
				return
		self.result.append(factory.make(ItemLibrary, Categories.library, name))
		print(f"WARNING: cannot locate library {name}")

	def addLinkerScript(self, fileName: str, factory: ItemFactory = _defaultItemFactory) -> None:
		"""Add a new linker script to the list."""

		for path in self.librarySearchPaths:
			fullPath = path / fileName
			if fullPath.is_file():
				self.result.append(factory.make(ItemPathOrString, Categories.linkerScript, fullPath))
				return
		self.result.append(factory.make(ItemPathOrString, Categories.linkerScript, fileName))
		print(f"WARNING: cannot locate linker script {fileName}")

	def addSearchPath(
		self,
		category: Categories,
		path: pathlib.Path,
		factory: ItemFactory = _defaultItemFactory,
	) -> None:
		"""Add a search path to a container."""

		searchPath = factory.make(ItemPath, category, path)
		self.result.append(searchPath)

	def addOutputPath(self, path: pathlib.Path, factory: ItemFactory = _defaultItemFactory) -> None:
		"""Add the output path."""

		self.result.append(factory.make(ItemPath, Categories.outputPath, self.cwd / path))

	@property
	def librarySearchPaths(self) -> typing.Iterable[pathlib.Path]:
		for entry in self.result:
			if entry.category == Categories.librarySearchPath:
				assert isinstance(entry, ItemPath)
				yield entry.path

	@property
	def includeSearchPaths(self) -> typing.Iterable[pathlib.Path]:
		for entry in self.result:
			if entry.category == Categories.includeSearchPath:
				assert isinstance(entry, ItemPath)
				yield entry.path

	@property
	def linkerScripts(self) -> typing.Iterable[pathlib.Path]:
		for entry in self.result:
			if entry.category == Categories.linkerScript:
				assert isinstance(entry, ItemPathOrString)
				if entry.hasPath:
					yield typing.cast(pathlib.Path, entry.pathOrValue)

	@property
	def libraries(self) -> typing.Iterable[pathlib.Path]:
		for entry in self.result:
			if entry.category == Categories.library:
				assert isinstance(entry, ItemPathOrString)
				if entry.hasPath:
					yield typing.cast(pathlib.Path, entry.pathOrValue)

	def _fallback(self, factory: ItemFactory, arg: str) -> None:
		if arg.endswith(".a"):
			self.addLibrary(arg, factory)
		else:
			print("Unhandled argument:", arg)
			self.result.append(factory.make(ItemString, Categories.unhandled, arg))

	def parse(self, cmdString: str) -> None:  # type: ignore
		super().parse(
			cmdString,
			[
				{
					r"-L": Processor(
						1,
						lambda factory, x: self.addSearchPath(Categories.librarySearchPath, pathlib.Path(x), factory),
					),
					r"-I": Processor(
						1,
						lambda factory, x: self.addSearchPath(Categories.includeSearchPath, pathlib.Path(x), factory),
					),
				},
				{
					r"-l": Processor(1, lambda factory, x: self.addLibrary(x, factory)),
					r"-T": Processor(1, lambda factory, x: self.addLinkerScript(x, factory)),
					r"-o": Processor(1, lambda factory, x: self.addOutputPath(x, factory)),
					**self.generateItemString(
						{
							r"-u": Categories.undefine,
							r"-D": Categories.define,
							r"-W": Categories.warning,
							r"-m": Categories.mode,
							r"-f": Categories.flag,
							r"-O": Categories.optimisation,
							r"-g": Categories.debug,
							r"-std": Categories.standard,
						}
					),
					**self.generateItem({r"-c": Categories.compileOnly}),
				},
			],
			self._fallback,
		)
