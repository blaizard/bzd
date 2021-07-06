import json
import typing
from pathlib import Path

from bzd.parser.parser import Parser as BaseParser
from bzd.parser.context import Context
from bzd.parser.element import Element, Sequence

from tools.bdl.grammar import Parser
from tools.bdl.visitors.process_inclusions import ProcessInclusions
from tools.bdl.visitors.map import Map, MapType
from tools.bdl.visitors.validation import Validation
from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.use import Use


class ObjectContext:

	def __init__(self,
		root: Path = Path(),
		usePath: typing.Optional[typing.List[Path]] = None,
		preprocessFormat: typing.Optional[str] = None) -> None:

		self.root = root
		self.usePath = [] if usePath is None else usePath
		self.preprocessFormat = "{}.o" if preprocessFormat is None else preprocessFormat
		self.dependencies: typing.List[Path] = []

	def pushDependency(self, path: Path) -> None:
		"""
		Push a dependency for this object.
		"""
		# Check for circular dependencies
		if path in self.dependencies:
			raise Exception("Circular dependency detected:\n{}".format("\n".join(
				[f.as_posix() for f in self.dependencies])))
		self.dependencies.append(path)

	def popDependency(self) -> None:
		"""
		Pop last dependency.
		"""
		self.dependencies.pop()

	def getPreprocessedPath(self, path: Path) -> Path:
		"""
		Return the preprocessed path from a BDL path.
		"""
		return self.root / Path(self.preprocessFormat.format(path.as_posix()))

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
		bdl = preprocessedPath.read_text(encoding="ascii")
		return Object.fromSerialize(str(bdl), objectContext=self)

	def preprocess(self, path: Path, includeDeps: bool = True) -> "Object":
		"""
		Preprocess a bdl file and save its output, or use the preprocessed file if present.
		"""

		if self.isPreprocessed(path=path):
			return self.loadPreprocess(path=path)

		# Push current dependency
		self.pushDependency(path)

		# Parse the input file.
		parser = Parser.fromPath(path)
		bdl = Object._makeObject(parser=parser, objectContext=self, includeDeps=includeDeps)

		# Save the preprocessed payload to a file.
		# Do not save when ignoring dependencies, as this creates un-complete views.
		if includeDeps:
			self.savePreprocess(path=path, object=bdl)

		# Pop dependency
		self.popDependency()

		return bdl


class Object:
	"""
	BDL object representation.
	"""

	def __init__(self, context: Context, parsed: Sequence, symbols: MapType, objectContext: ObjectContext) -> None:
		self.context = context
		self.parsed = parsed
		self.symbols = symbols
		self.objectContext = objectContext
		# Memoized buffer holding the elements, this is constructed while being used.
		self.elements: typing.Dict[str, Element] = {}

	@staticmethod
	def _makeObject(parser: BaseParser, objectContext: ObjectContext, includeDeps: bool) -> "Object":
		"""
		Helper to make an object from a parser.
		"""
		data = parser.parse()

		# Look for all includes, this stage ensures that dependencies are present and preprocessed.
		if includeDeps:
			ProcessInclusions(objectContext=objectContext).visit(data)

		# Validation step
		Validation().visit(data)

		# Generate the symbol map
		symbols = Map().visit(data)

		return Object(context=parser.context, parsed=data, symbols=symbols, objectContext=objectContext)

	@staticmethod
	def fromContent(content: str, objectContext: typing.Optional[ObjectContext] = None) -> "Object":
		"""
		Make an object from a the content of a bdl file.
		This is mainly used for testing purpose.
		"""

		parser = Parser(content)
		return Object._makeObject(parser=parser,
			objectContext=ObjectContext() if objectContext is None else objectContext,
			includeDeps=objectContext is not None)

	@staticmethod
	def fromSerialize(data: str, objectContext: ObjectContext) -> "Object":
		"""
		Make an object from a serialized payload.
		"""

		payload = json.loads(data)
		context = Context.fromSerialize(payload["context"])
		return Object(context=context,
			parsed=Sequence.fromSerialize(payload["parsed"], context),
			symbols=payload["symbols"],
			objectContext=objectContext)

	def serialize(self) -> str:
		"""
		Serialize the current object.
		"""

		return json.dumps(
			{
			"context": self.context.serialize(),
			"parsed": self.parsed.serialize(),
			"symbols": self.symbols
			},
			separators=(",", ":"))

	def registerUse(self, entity: Use) -> None:
		"""
		Update the content of this object with an existing one.
		"""

		entity.assertTrue(condition=self.objectContext.isPreprocessed(entity.path),
			message="Cannot find preprocessed entity for '{}'.".format(entity.path))

		bdl = self.objectContext.loadPreprocess(path=entity.path)
		self.registerSymbols(bdl.symbols)

	def registerSymbols(self, symbols: MapType) -> None:
		"""
		Register multiple symbols.
		"""
		for key, element in symbols.items():
			assert key not in self.symbols, "Symbol conflict '{}'.".format(key)
			self.symbols[key] = element

	def getElement(self, fqn: str) -> typing.Optional[Element]:

		if fqn not in self.symbols:
			return None

		# Not memoized
		if fqn not in self.elements:
			element = Element.fromSerialize(element=self.symbols[fqn])
			self.elements[fqn] = element

		# Return the element
		return self.elements[fqn]

	def getElementFromName(self, name: str, namespace: typing.List[str]) -> typing.Optional[Element]:

		# Look for a symbol matchparsed
		namespace = namespace.copy()
		while True:
			fqn = Map.makeFQN(name=name, namespace=namespace)
			if fqn in self.symbols:
				break
			if not namespace:
				return None
			namespace.pop()

		# Match found
		return self.getElement(fqn=fqn)

	def getElementFromType(self, entity: Type, namespace: typing.List[str]) -> typing.Optional[Element]:

		if entity.isFQN:
			return self.getElement(fqn=entity.kind)
		return self.getElementFromName(name=entity.kind, namespace=namespace)

	def printSymbols(self) -> None:
		"""
		Print the symbol map.
		"""
		for symbol, data in self.symbols.items():
			print("{}".format(symbol))
