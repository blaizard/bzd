import json
import typing
import copy
from pathlib import Path

from bzd.parser.parser import Parser as BaseParser
from bzd.parser.context import Context
from bzd.parser.element import Element, Sequence

from bdl.grammar import Parser
from bdl.visitors.symbol_map import SymbolMap
from bdl.visitors.symbol_tree import SymbolTree
from bdl.visitors.preprocess.process_inclusions import ProcessInclusions
from bdl.visitors.preprocess.build import Build
from bdl.visitors.preprocess.validation import Validation
from bdl.entities.all import EntityType
from bdl.entities.impl.fragment.symbol import Symbol
from bdl.entities.impl.use import Use


class ObjectContext:

	def __init__(
	    self,
	    preprocessFormat: typing.Optional[str] = None,
	    searchFormats: typing.Optional[typing.List[str]] = None,
	    resolve: bool = False,
	    composition: bool = False,
	) -> None:
		"""
        Args:
                resolve: Resolve all symbols.
                composition: Include composition stage (for testing purpose only).
        """

		self.preprocessFormat = "{}.o" if preprocessFormat is None else preprocessFormat
		self.searchFormats = searchFormats if searchFormats else []
		self.sources: typing.List[Path] = []
		self.resolve = resolve
		self.composition = composition

	def pushSource(self, source: str) -> None:
		"""Push a dependency for this object."""

		path = self.getPathFromSource(source)
		# Check for circular dependencies
		if path in self.sources:
			raise Exception("Circular dependency detected:\n{}".format("\n".join([f.as_posix() for f in self.sources])))
		self.sources.append(path)

	def popSource(self) -> None:
		"""Pop last dependency."""

		self.sources.pop()

	def getSource(self) -> typing.Optional[Path]:
		"""Get the current source file path."""

		return self.sources[-1] if len(self.sources) > 0 else None

	def getPathFromSource(self, source: str) -> Path:
		"""Extract the path name from the source."""

		splitted = source.split("@")
		assert len(splitted) <= 2, f"The source '{source}' is malformed."
		return Path(splitted[0])

	def getPreprocessedPathFromSource(self, source: str) -> Path:
		"""Extract the preprocess path name from the source."""

		splitted = source.split("@")
		assert len(splitted) <= 2, f"The source '{source}' is malformed."
		return (Path(splitted[1]) if len(splitted) > 1 else Path(self.preprocessFormat.format(source)))

	def findPreprocess(self, source: str) -> typing.Optional[Path]:
		"""Search for the preprocessed object file if any."""

		def getPreprocessedPath(source: str, path: Path) -> typing.Iterator[Path]:
			yield self.getPreprocessedPathFromSource(source=source)
			for fmt in self.searchFormats:
				yield Path(fmt.format(path))

		path = self.getPathFromSource(source=source)
		for preprocessed in getPreprocessedPath(source=source, path=path):
			if preprocessed.is_file():
				# Compare its modification time with the source file.
				if path.stat().st_mtime > preprocessed.stat().st_mtime:
					return None
				return preprocessed
		return None

	def isPreprocessed(self, source: str) -> bool:
		"""Check if a BDL file has a preprocessed counter-part."""

		path = self.getPathFromSource(source=source)
		preprocessed = self.getPreprocessedPathFromSource(source=source)
		if preprocessed.is_file():
			# If the source file is present, compare its modification time with the preprocessed.
			if path.is_file():
				if path.stat().st_mtime > preprocessed.stat().st_mtime:
					return False
			return True
		return False

	def savePreprocess(self, source: str, object: "Object") -> None:
		"""Save the serialized content of preprocessed object."""

		content = object.serialize()
		preprocessedPath = self.getPreprocessedPathFromSource(source=source)
		preprocessedPath.write_text(content, encoding="ascii")

	def loadPreprocess(self, preprocess: Path) -> "Object":
		"""Read a serialized preprocessed file and return it."""

		data = preprocess.read_text(encoding="ascii")

		# Unserialize the data
		payload = json.loads(data)
		context = Context.fromSerialize(payload["context"])
		symbols = SymbolMap.fromSerialize(payload["symbols"])
		return Object(
		    context=context,
		    symbols=symbols,
		    tree=SymbolTree.fromSerialize(payload["tree"], symbols),
		)

	def preprocess(self, source: str, namespace: typing.Optional[typing.List[str]] = None) -> "Object":
		"""Preprocess a bdl file and save its output, or use the preprocessed file if present.

        Args:
                source: The source file the be preprocessed.
                namespace: An optional wrapping namespace to be used for this file.
        """

		maybePreprocess = self.findPreprocess(source=source)
		if maybePreprocess:
			return self.loadPreprocess(preprocess=maybePreprocess)

		# Push current dependency
		self.pushSource(source=source)

		# Parse the input file.
		path = self.getPathFromSource(source=source)
		parser = Parser.fromPath(path)
		bdl = Object._makeObject(parser=parser, namespace=namespace, objectContext=self)

		# Save the preprocessed payload to a file.
		# Do not save when ignoring dependencies, as this creates un-complete views.
		if self.resolve:
			self.savePreprocess(source=source, object=bdl)

		# Pop dependency
		self.popSource()

		return bdl


class Object:
	"""BDL object representation."""

	def __init__(self, context: Context, symbols: SymbolMap, tree: SymbolTree) -> None:
		self.context = context
		self.symbols = symbols
		self.tree = tree

	@staticmethod
	def _makeObject(
	    parser: BaseParser,
	    objectContext: ObjectContext,
	    namespace: typing.Optional[typing.List[str]] = None,
	) -> "Object":
		"""Helper to make an object from a parser."""

		data = parser.parse()

		# Look for all includes, this stage ensures that dependencies are present and preprocessed.
		if objectContext.resolve:
			ProcessInclusions(objectContext=objectContext).visit(data)

		# Generate the symbol map
		build = Build(namespace=namespace, objectContext=objectContext)
		build.visit(data)

		# Validation step
		Validation().visit(data)

		return Object(
		    context=parser.context,
		    symbols=build.getSymbolMap(),
		    tree=build.getSymbolTree(),
		)

	@staticmethod
	def fromContent(content: str, objectContext: typing.Optional[ObjectContext] = None) -> "Object":
		"""Make an object from a the content of a bdl file.

        This is mainly used for testing purpose.
        """

		parser = Parser(content)
		return Object._makeObject(
		    parser=parser,
		    objectContext=objectContext if objectContext else ObjectContext(),
		)

	def entity(self, fqn: str) -> EntityType:
		"""Convinience function to lookup for an entity based on its FQN and return it.
        If the symbol does not exists, this will throw."""

		maybeEntity = self.symbols.getEntity(fqn)
		assert bool(maybeEntity), maybeEntity.error
		return maybeEntity.value

	def serialize(self) -> str:
		"""Serialize the current object."""

		return json.dumps(
		    {
		        "context": self.context.serialize(),
		        "symbols": self.symbols.serialize(),
		        "tree": self.tree.serialize(),
		    },
		    separators=(",", ":"),
		)

	def __repr__(self) -> str:
		"""Print an object (for debug purpose only)."""

		content = ""
		content += "--- Symbols\n"
		content += "".join(["\t{}\n".format(line) for line in str(self.symbols).split("\n")])
		content += "--- Tree\n"
		content += "".join(["\t{}\n".format(line) for line in str(self.tree).split("\n")])

		return content
