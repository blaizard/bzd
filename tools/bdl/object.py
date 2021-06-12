import json
import typing
from pathlib import Path

from bzd.parser.element import Element, Sequence

from tools.bdl.grammar import Parser
from tools.bdl.visitors.map import Map, MapType
from tools.bdl.visitors.validation import Validation
from tools.bdl.entities.impl.fragment.type import Type


class Object:
	"""
	BDL object representation.
	"""

	def __init__(self, parsed: Sequence, symbols: MapType) -> None:
		self.parsed = parsed
		self.symbols = symbols
		self.elements: typing.Dict[str, Element] = {}

	@staticmethod
	def fromPath(path: Path) -> "Object":
		"""
		Make an object from a bdl path file.
		"""

		# Parse the input file
		data = Parser.fromPath(path).parse()

		# Validation step
		Validation().visit(data)

		# Generate the symbol map
		symbols = Map().visit(data)

		return Object(parsed=data, symbols=symbols)

	@staticmethod
	def fromSerialize(data: str) -> "Object":
		"""
		Make an object from a serialized payload.
		"""

		payload = json.loads(data)
		return Object(parsed=Sequence.fromSerialize(payload["parsed"]), symbols=payload["symbols"])

	def serialize(self) -> str:
		"""
		Serialize the current object.
		"""

		return json.dumps({"parsed": self.parsed.serialize(), "symbols": self.symbols}, separators=(",", ":"))

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

		# Look for a symbol match
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
