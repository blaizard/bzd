import json
import typing
from pathlib import Path

from bzd.parser.parser import Parser as BaseParser
from bzd.parser.context import Context
from bzd.parser.element import Element, Sequence

from tools.bdl.grammar import Parser
from tools.bdl.visitors.map import Map, MapType
from tools.bdl.visitors.validation import Validation
from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.use import Use


class Object:
	"""
	BDL object representation.
	"""

	def __init__(self, context: Context, parsed: Sequence, symbols: MapType,
			usePath: typing.Optional[typing.List[Path]] = None,
			preprocessFormat: typing.Optional[str] = None) -> None:
		self.context = context
		self.parsed = parsed
		self.symbols = symbols
		self.usePath = [] if usePath is None else usePath
		self.preprocessFormat = "{}.o" if preprocessFormat is None else preprocessFormat
		# Memoized buffer holding the elements, this is constructed while being used.
		self.elements: typing.Dict[str, Element] = {}

	@staticmethod
	def _makeObject(parser: BaseParser, **kwargs: typing.Any) -> "Object":
		"""
		Helper to make an opbject from a parser.
		"""
		data = parser.parse()

		# Validation step
		Validation().visit(data)

		# Generate the symbol map
		symbols = Map().visit(data)

		return Object(context=parser.context, parsed=data, symbols=symbols, **kwargs)

	@staticmethod
	def fromContent(content: str, **kwargs: typing.Any) -> "Object":
		"""
		Make an object from a the content of a bdl file.
		This is mainly used for testing purpose.
		"""

		parser = Parser(content)
		return Object._makeObject(parser=parser, **kwargs)

	@staticmethod
	def fromSerializePath(path: Path, **kwargs: typing.Any) -> "Object":
		"""
		From a serialized object.
		"""

		bdl = path.read_text(encoding="ascii")
		return Object.fromSerialize(str(bdl), **kwargs)

	@staticmethod
	def fromPath(path: Path, **kwargs: typing.Any) -> "Object":
		"""
		Make an object from a bdl path file.
		"""
		# Parse the input file
		parser = Parser.fromPath(path)
		return Object._makeObject(parser=parser, **kwargs)

	@staticmethod
	def fromSerialize(data: str, **kwargs: typing.Any) -> "Object":
		"""
		Make an object from a serialized payload.
		"""

		payload = json.loads(data)
		context = Context.fromSerialize(payload["context"])
		return Object(context=context,
			parsed=Sequence.fromSerialize(payload["parsed"], context),
			symbols=payload["symbols"],
			**kwargs)

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

		preprocessPath = self.preprocessFormat.format(entity.path.as_posix())
		for root in self.usePath:
			if (root / preprocessPath).is_file():
				bdl = Object.fromSerializePath(root / preprocessPath)
				self.registerSymbols(bdl.symbols)
				return

		entity.assertTrue(condition=False, message="Cannot find path: '{}'.".format(entity.path))

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
