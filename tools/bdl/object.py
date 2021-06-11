import json
from pathlib import Path

from bzd.parser.element import Sequence

from tools.bdl.grammar import Parser
from tools.bdl.visitors.map import Map, MapType
from tools.bdl.visitors.validation import Validation


class Object:
	"""
	BDL object representation.
	"""

	def __init__(self, parsed: Sequence, symbols: MapType) -> None:
		self.parsed = parsed
		self.symbols = symbols

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
