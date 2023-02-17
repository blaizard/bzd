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
	# If this is a writer endpoint or not.
	isWriter: bool
	# The type of symbol for this endpoint.
	symbol: Symbol
	# The connections associated with this endpoint.
	connections: typing.Set[EndpointId] = dataclasses.field(default_factory=set)
	# The maximum history required by this endpoint.
	history: int = 1
	# If it is intended to accept multiple connection on the same endpoint.
	multi: bool = False


@dataclasses.dataclass
class ConnectionArguments:
	# The writer associated with this connection.
	writer: EntityExpression
	# The set of readers associated with the writer.
	readers: typing.Set[EntityExpression]


class Connections:

	def __init__(self, symbols: SymbolMap) -> None:
		self.resolver = symbols.makeResolver()
		# List of all available endpoints. Endpoints are the end of each connection.
		self.endpoints: typing.Dict[EndpointId, Metadata] = {}
		self.connections: typing.List[ConnectionArguments] = []

	def addEndpoint(self, this: EntityExpression, endpoint: EntityExpression) -> None:
		"""Register a new endpoint."""

		endpoint.assertTrue(condition=endpoint.isName, message="Endpoint must have a name.")
		endpoint.assertTrue(condition=endpoint.isSymbol, message="Endpoint must have a symbol.")

		identifier = EndpointId(this=this.fqn, name=endpoint.name)
		this.assertTrue(condition=identifier not in self.endpoints,
		                message=f"The endpoint '{this.fqn}.{endpoint.name}' is already registered.")
		self.endpoints[identifier] = Metadata(isWriter=not endpoint.const,
		                                      symbol=endpoint.symbol,
		                                      multi=endpoint.isVarArgs)

	def addConnection(self, writer: EndpointId, reader: EndpointId) -> None:
		"""Add a new connection between a writer and a reader."""

		# Make sure the endpoints exists.
		assert writer in self.endpoints, f"The writer '{writer}' is not registered, only the followings are: {list(self.endpoints.keys())}."
		assert reader in self.endpoints, f"The reader '{reader}' is not registered, only the followings are: {list(self.endpoints.keys())}."

		# Check const correctness.
		assert self.endpoints[writer].isWriter, f"The writer '{writer}' must not be marked as const."
		assert not self.endpoints[reader].isWriter, f"The reader '{reader}' must be marked as const."

		# Sanity checks.
		assert writer != reader, f"A connection cannot connect to itself: '{writer}' -> '{reader}'."
		assert len(self.endpoints[reader].connections) == 0 or self.endpoints[
		    reader].multi, f"Readers can only have a single connection, '{reader}' is already connected to {self.endpoints[reader].connections}."
		assert reader not in self.endpoints[
		    writer].connections, f"Connection between '{writer}' and '{reader}' is defined multiple times."

		self.endpoints[reader].connections.add(writer)
		self.endpoints[writer].connections.add(reader)

		# Make sure the types are compatibles.
		writerTypeFQN = self.endpoints[writer].symbol.underlyingTypeFQN
		readerTypeFQN = self.endpoints[reader].symbol.underlyingTypeFQN
		assert writerTypeFQN == readerTypeFQN or writerTypeFQN == "Any" or readerTypeFQN == "Any", f"A connection must be made between the same types, not '{writerTypeFQN}' and '{readerTypeFQN}'."

	def add(self, writer: EntityExpression, *readers: EntityExpression) -> None:
		"""Register a new connection for later processing."""
		self.connections.append(ConnectionArguments(writer=writer, readers=set(readers)))

	def getIdentifiers(self, entity: EntityExpression, identifiers: typing.Set[EndpointId],
	                   description: str) -> typing.Set[EndpointId]:
		"""Get the list of identifiers filtered by the entity."""

		# If the entity contains a regular expression.
		if entity.isRegexpr:
			matches = entity.regexpr.match(identifiers)
			entity.assertTrue(condition=bool(matches), message="There are no matches from this regular expression.")
			return matches

		# Else it should point to a signal input.
		entity.assertTrue(condition=entity.isSymbol, message="Must be a regexpr of a symbol.")
		entity.assertTrue(condition=entity.isLValue, message="Must be a reference to another object.")

		# If the entity represents a special type.
		entityType = entity.symbol.getEntityUnderlyingTypeResolved(resolver=self.resolver)
		if "bzd.platform.Recorder" in entityType.getParents():
			identifier = EndpointId(str(entity.symbol), "readers")
		else:
			identifier = EndpointId.fromSymbol(entity.symbol)
		entity.assertTrue(condition=identifier in identifiers, message=f"'{identifier}' is not a valid {description}.")

		return {identifier}

	def close(self) -> None:
		"""Process all the connections."""

		# List all available identifiers.
		allWriters = {identifier for identifier, metadata in self.endpoints.items() if metadata.isWriter}
		allReaders = {identifier for identifier, metadata in self.endpoints.items() if not metadata.isWriter}

		for connection in self.connections:

			# Match the identifiers.
			writers = self.getIdentifiers(connection.writer, allWriters, "writer IO")
			readers: typing.Set[EndpointId] = set()
			for entity in connection.readers:
				matches = self.getIdentifiers(entity, allReaders, "reader IO")
				entity.assertTrue(condition=bool(matches - readers),
				                  message=f"This expression does not match any IO readers.")
				readers.update(matches)

			# Register the connections.
			for writer in writers:
				for reader in readers:
					try:
						self.addConnection(writer=writer, reader=reader)
					except AssertionError as e:
						connection.writer.error(str(e))

	def __repr__(self) -> str:
		content = []
		for writer, metadata in self.endpoints.items():
			content += [f"{writer} => {metadata.connections}"]
		return "\n".join(content)
