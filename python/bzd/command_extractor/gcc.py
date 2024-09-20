import typing
import dataclasses
import pathlib
import shlex
import re
import os
import enum

from bzd.command_extractor.common import (
    CommandExtractor,
    Processor,
    Item,
    ItemString,
    ItemPath,
    ItemPathOrString,
    ItemFactory,
)


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


class ItemLibrary(ItemPathOrString):  # type: ignore

	@property
	def name(self) -> str:
		"""Get the library name."""
		return re.sub(r"lib(.*)\.a", r"\1", pathlib.Path(self.pathOrValue).name)


class CommandExtractorGcc(CommandExtractor):  # type: ignore

	def __init__(
	    self,
	    cwd: pathlib.Path,
	    includeSearchPaths: typing.List[pathlib.Path] = [],
	    libSearchPaths: typing.List[pathlib.Path] = [],
	) -> None:
		super().__init__()
		self.cwd = cwd
		for path in [cwd] + includeSearchPaths:
			self.addSearchPath(Categories.includeSearchPath, path)
		for path in [cwd] + libSearchPaths:
			self.addSearchPath(Categories.librarySearchPath, path)

	def addLibrary(self, name: str, factory: ItemFactory = ItemFactory()) -> None:
		"""Add a new library the the list."""

		fileName = (name if re.match(r"lib.*\.a", pathlib.Path(name).name) else "lib{}.a".format(name))
		for path in self.librarySearchPaths:
			fullPath = path / fileName
			if fullPath.is_file():
				self.result.append(factory.make(ItemLibrary, Categories.library, fullPath))
				return
		self.result.append(factory.make(ItemLibrary, Categories.library, name))
		print("WARNING: cannot locate library {}".format(name))

	def addLinkerScript(self, fileName: str, factory: ItemFactory = ItemFactory()) -> None:
		"""Add a new linker script the the list."""

		for path in self.librarySearchPaths:
			fullPath = path / fileName
			if fullPath.is_file():
				self.result.append(factory.make(ItemPathOrString, Categories.linkerScript, fullPath))
				return
		self.result.append(factory.make(ItemPathOrString, Categories.linkerScript, fileName))
		print("WARNING: cannot locate linker script {}".format(fileName))

	def addSearchPath(
	        self,
	        category: Categories,
	        path: pathlib.Path,
	        factory: ItemFactory = ItemFactory(),
	) -> None:
		"""Add a search path to a container."""

		searchPath = factory.make(ItemPath, category, path)
		self.result.append(searchPath)

	def addOutputPath(self, path: pathlib.Path, factory: ItemFactory = ItemFactory()) -> None:
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

	def parse(self, cmdString: str) -> None:
		super().parse(
		    cmdString,
		    {
		        r"-L":
		            Processor(
		                1,
		                lambda factory, x: self.addSearchPath(Categories.librarySearchPath, pathlib.Path(x), factory),
		            ),
		        r"-l":
		            Processor(1, lambda factory, x: self.addLibrary(x, factory)),
		        r"-T":
		            Processor(1, lambda factory, x: self.addLinkerScript(x, factory)),
		        r"-I":
		            Processor(
		                1,
		                lambda factory, x: self.addSearchPath(Categories.includeSearchPath, pathlib.Path(x), factory),
		            ),
		        r"-o":
		            Processor(1, lambda factory, x: self.addOutputPath(x, factory)),
		        **self.generateItemString({
		            r"-u": Categories.undefine,
		            r"-D": Categories.define,
		            r"-W": Categories.warning,
		            r"-m": Categories.mode,
		            r"-f": Categories.flag,
		            r"-O": Categories.optimisation,
		            r"-g": Categories.debug,
		            r"-std": Categories.standard,
		        }),
		        **self.generateItem({r"-c": Categories.compileOnly}),
		    },
		    self._fallback,
		)
