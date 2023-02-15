import typing
import dataclasses

from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.entities.impl.entity import EntityExpression
from tools.bdl.entities.impl.fragment.symbol import Symbol
from tools.bdl.entities.impl.types import Category


@dataclasses.dataclass(frozen=True)
class EndpointId:

	# The fqn of the object instance.
	this: str
	# The name of the property.
	name: str

	@staticmethod
	def fromSymbol(symbol: Symbol) -> "EndpointId":
		symbol.assertTrue(condition=symbol.isThis, message=f"The symbol must point to an instance member.")
		return EndpointId(this=symbol.this, name=symbol.propertyName)

	def __repr__(self) -> str:
		return f"{self.this}.{self.name}"


@dataclasses.dataclass
class Metadata:
	writter: bool
	# The type of symbol for this endpoint.
	symbol: Symbol
	# The maximum history required by this endpoint.
	history: int = 1
	# If it is intended to accept multiple connection on the same endpoint.
	multi: bool = False


class Connections:

	def __init__(self, symbols: SymbolMap) -> None:
		self.resolver = symbols.makeResolver()
		# List of all available endpoints. Endpoints are the end of each connection.
		self.endpoints: typing.Dict[EndpointId, Metadata] = {}
		self.groups: typing.Dict[EndpointId, typing.Set[EndpointId]] = {}
		self.recorders: typing.Dict[Symbol, typing.Set[EntityExpression]] = {}

	def addRecorder(self, recorder: Symbol, output: EntityExpression) -> None:
		"""Register a new recorder."""

		if recorder not in self.recorders:
			self.recorders[recorder] = set()
		self.recorders[recorder].add(output)

	def addEndpoint(self, this: EntityExpression, endpoint: EntityExpression) -> None:
		"""Register a new endpoint."""

		endpoint.assertTrue(condition=endpoint.isName, message="Endpoint must have a name.")
		endpoint.assertTrue(condition=endpoint.isSymbol, message="Endpoint must have a symbol.")

		identifier = EndpointId(this=this.fqn, name=endpoint.name)
		this.assertTrue(condition=identifier not in self.endpoints,
		                message=f"The endpoint '{this.fqn}.{endpoint.name}' is already registered.")
		self.endpoints[identifier] = Metadata(writter=not endpoint.const,
		                                      symbol=endpoint.symbol,
		                                      multi=endpoint.isVarArgs)

	def addConnection(self, writter: EndpointId, reader: EndpointId) -> None:
		"""Add a new connection between a writter and a reader."""

		# Make sure the endpoints exists.
		assert writter in self.endpoints, f"The writter '{writter}' is not registered, only the followings are: {list(self.endpoints.keys())}."
		assert reader in self.endpoints, f"The reader '{reader}' is not registered, only the followings are: {list(self.endpoints.keys())}."

		# Check const correctness.
		assert self.endpoints[writter].writter, f"The writter '{writter}' must not be marked as const."
		assert not self.endpoints[reader].writter, f"The reader '{reader}' must be marked as const."

		# Sanity checks.
		assert writter != reader, f"A connection cannot connect to itself: '{writter}' -> '{reader}'."
		assert not (writter in self.groups and reader in self.groups[writter]
		            ), f"Connection between '{writter}' and '{reader}' is defined multiple times."

		if writter not in self.groups:
			self.groups[writter] = set()
		self.groups[writter].add(reader)

	def add(self, input: EntityExpression, output: EntityExpression) -> None:
		"""Register a new connection to the connection map."""

		input.assertTrue(condition=input.isLValue, message="First argument must be a reference to another object.")

		# If the input is a bzd.platform.Recorder.
		inputEntityType = input.symbol.getEntityUnderlyingTypeResolved(resolver=self.resolver)
		if "bzd.platform.Recorder" in inputEntityType.getParents():
			self.addRecorder(recorder=input.symbol, output=output)
			return

		output.assertTrue(condition=output.isLValue, message="Second argument must be a reference to another object.")

		input.assertTrue(
		    condition=input.underlyingTypeFQN == output.underlyingTypeFQN,
		    message=
		    f"Connections cannot only be made between same types, not {input.underlyingTypeFQN} and {output.underlyingTypeFQN}."
		)

		try:
			self.addConnection(writter=EndpointId.fromSymbol(input.symbol), reader=EndpointId.fromSymbol(output.symbol))
		except AssertionError as e:
			input.error(str(e))

	def close(self) -> None:
		# List of all writters.
		identifiers = {identifier for identifier, metadata in self.endpoints.items() if metadata.writter}
		for recorder, entities in self.recorders.items():

			matches: typing.Set[EndpointId] = set()
			for entity in entities:
				# If the entity contains a regular expression.
				if entity.isRegexpr:
					newMatches = entity.regexpr.match(identifiers)
					entity.assertTrue(condition=bool(newMatches),
					                  message="There are no matches from this regular expression.")
					entity.assertTrue(condition=bool(newMatches - matches),
					                  message=f"All matches are alreay sets: {newMatches}")
					matches.update(newMatches)
				# Else it should point to a signal input.
				else:
					entity.assertTrue(condition=entity.isSymbol, message="Must be a regexpr of a symbol.")
					entity.assertTrue(condition=entity.isLValue, message="Must be a reference to another object.")
					identifier = EndpointId.fromSymbol(entity.symbol)
					entity.assertTrue(condition=identifier in identifiers,
					                  message="A recorder must be connected to a IO generator.")
					entity.assertTrue(condition=identifier not in matches, message=f"This entity is already twice.")
					matches.add(identifier)

			# Register the connections.
			recorder.assertTrue(condition=bool(matches), message="The recorder is not associated with any IO.")
			for identifier in matches:
				self.addConnection(writter=identifier, reader=EndpointId(this=recorder.fqn, name="inputs"))

	def __repr__(self) -> str:
		content = []
		for writter, readers in self.groups.items():
			content += [f"{writter} => {readers}"]
		return "\n".join(content)
