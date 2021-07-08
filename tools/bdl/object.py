import json
import typing
from pathlib import Path

from bzd.parser.parser import Parser as BaseParser
from bzd.parser.context import Context
from bzd.parser.element import Element, Sequence

from tools.bdl.grammar import Parser
from tools.bdl.visitors.preprocess.process_inclusions import ProcessInclusions
from tools.bdl.visitors.preprocess.build import Build
from tools.bdl.visitors.preprocess.symbol_map import SymbolMap
from tools.bdl.visitors.preprocess.validation import Validation
from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.use import Use


class ObjectContext:

	def __init__(self, preprocessFormat: typing.Optional[str] = None, includeDeps: bool = False) -> None:

		self.preprocessFormat = "{}.o" if preprocessFormat is None else preprocessFormat
		self.sources: typing.List[Path] = []
		self.includeDeps = includeDeps

	def pushSource(self, path: Path) -> None:
		"""
		Push a dependency for this object.
		"""
		# Check for circular dependencies
		if path in self.sources:
			raise Exception("Circular dependency detected:\n{}".format("\n".join([f.as_posix() for f in self.sources])))
		self.sources.append(path)

	def popSource(self) -> None:
		"""
		Pop last dependency.
		"""
		self.sources.pop()

	def getSource(self) -> typing.Optional[Path]:
		"""
		Get the current source file path.
		"""
		return self.sources[-1] if len(self.sources) > 0 else None

	def getPreprocessedPath(self, path: Path) -> Path:
		"""
		Return the preprocessed path from a BDL path.
		"""
		return Path(self.preprocessFormat.format(path.as_posix()))

	def isPreprocessed(self, path: Path) -> bool:
		"""
		Check if a BDL file has a preprocessed counter-part.
		"""

		preprocessed = self.getPreprocessedPath(path=path)
		if preprocessed.is_file():
			return True
		return False

	def savePreprocess(self, path: Path, object: "Object") -> None:
		"""
		Save the serialized content of preprocessed object.
		"""

		content = object.serialize()
		self.getPreprocessedPath(path=path).write_text(content, encoding="ascii")

	def loadPreprocess(self, path: Path) -> "Object":
		"""
		Read a serialized preprocessed file and return it.
		"""

		preprocessedPath = self.getPreprocessedPath(path=path)
		data = preprocessedPath.read_text(encoding="ascii")

		# Unserialize the data
		payload = json.loads(data)
		context = Context.fromSerialize(payload["context"])
		return Object(context=context,
			parsed=Sequence.fromSerialize(payload["parsed"], context),
			symbols=SymbolMap.fromSerialize(payload["symbols"]))

	def preprocess(self, path: Path) -> "Object":
		"""
		Preprocess a bdl file and save its output, or use the preprocessed file if present.
		"""

		if self.isPreprocessed(path=path):
			return self.loadPreprocess(path=path)

		# Push current dependency
		self.pushSource(path)

		# Parse the input file.
		parser = Parser.fromPath(path)
		bdl = Object._makeObject(parser=parser, objectContext=self)

		# Save the preprocessed payload to a file.
		# Do not save when ignoring dependencies, as this creates un-complete views.
		if self.includeDeps:
			self.savePreprocess(path=path, object=bdl)

		# Pop dependency
		self.popSource()

		return bdl


class Object:
	"""
	BDL object representation.
	"""

	def __init__(self, context: Context, parsed: Sequence, symbols: SymbolMap) -> None:
		self.context = context
		self.parsed = parsed
		self.symbols = symbols

	@staticmethod
	def _makeObject(parser: BaseParser, objectContext: ObjectContext) -> "Object":
		"""
		Helper to make an object from a parser.
		"""
		data = parser.parse()

		# Look for all includes, this stage ensures that dependencies are present and preprocessed.
		if objectContext.includeDeps:
			ProcessInclusions(objectContext=objectContext).visit(data)

		# Validation step
		Validation().visit(data)

		# Generate the symbol map
		resolve = Build(objectContext=objectContext)
		resolve.visit(data)
		symbols = resolve.getSymbolMap()

		return Object(context=parser.context, parsed=data, symbols=symbols)

	@staticmethod
	def fromContent(content: str) -> "Object":
		"""
		Make an object from a the content of a bdl file.
		This is mainly used for testing purpose.
		"""

		parser = Parser(content)
		return Object._makeObject(parser=parser, objectContext=ObjectContext())

	def serialize(self) -> str:
		"""
		Serialize the current object.
		"""

		return json.dumps(
			{
			"context": self.context.serialize(),
			"parsed": self.parsed.serialize(),
			"symbols": self.symbols.serialize()
			},
			separators=(",", ":"))

	def getElementFromType(self,
		entity: Type,
		namespace: typing.List[str],
		category: typing.Optional[str] = None) -> typing.Optional[Element]:

		if entity.isFQN:
			return self.symbols.getElement(fqn=entity.kind, category=category)
		return self.symbols.getElementFromName(name=entity.kind, namespace=namespace, category=category)

	def __repr__(self) -> str:
		"""
		Print an object (for debug purpose only).
		"""
		content = ""
		content += "--- Symbols\n"
		content += "".join(["\t{}\n".format(line) for line in str(self.symbols).split("\n")])
		content += "--- Parsed\n"
		content += "".join(["\t{}\n".format(line) for line in str(self.parsed).split("\n")])

		return content
