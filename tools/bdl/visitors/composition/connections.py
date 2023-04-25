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
	# If this is a source endpoint or not.
	isSource: bool
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
	# The source associated with this connection.
	source: EntityExpression
	# The set of sinks associated with the source.
	sinks: typing.Set[EntityExpression]


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
		self.endpoints[identifier] = Metadata(isSource=not endpoint.const,
		                                      symbol=endpoint.symbol,
		                                      multi=endpoint.isVarArgs)

	def addConnection(self, source: EndpointId, sink: EndpointId) -> None:
		"""Add a new connection between a source and a sink."""

		# Make sure the endpoints exists.
		assert source in self.endpoints, f"The source '{source}' is not registered, only the followings are: {list(self.endpoints.keys())}."
		assert sink in self.endpoints, f"The sink '{sink}' is not registered, only the followings are: {list(self.endpoints.keys())}."

		# Check const correctness.
		assert self.endpoints[source].isSource, f"The source '{source}' must not be marked as const."
		assert not self.endpoints[sink].isSource, f"The sink '{sink}' must be marked as const."

		# Sanity checks.
		assert source != sink, f"A connection cannot connect to itself: '{source}' -> '{sink}'."
		assert len(self.endpoints[sink].connections) == 0 or self.endpoints[
		    sink].multi, f"Sinks can only have a single connection, '{sink}' is already connected to {self.endpoints[sink].connections}."
		assert sink not in self.endpoints[
		    source].connections, f"Connection between '{source}' and '{sink}' is defined multiple times."

		self.endpoints[sink].connections.add(source)
		self.endpoints[source].connections.add(sink)

		# Make sure the types are compatibles.
		sourceTypeFQN = self.endpoints[source].symbol.underlyingTypeFQN
		sinkTypeFQN = self.endpoints[sink].symbol.underlyingTypeFQN
		assert sourceTypeFQN == sinkTypeFQN or sourceTypeFQN == "Any" or sinkTypeFQN == "Any", f"A connection must be made between the same types, not '{sourceTypeFQN}' and '{sinkTypeFQN}'."

	def add(self, source: EntityExpression, *sinks: EntityExpression) -> None:
		"""Register a new connection for later processing."""
		self.connections.append(ConnectionArguments(source=source, sinks=set(sinks)))

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
		if "bzd.Recorder" in entityType.getParents():
			identifier = EndpointId(str(entity.symbol), "sinks")
		else:
			identifier = EndpointId.fromSymbol(entity.symbol)
		entity.assertTrue(
		    condition=identifier in identifiers,
		    message=f"'{identifier}' is not a valid {description}, valid {description} are: {identifiers}.")

		return {identifier}

	def close(self) -> None:
		"""Process all the connections."""

		# List all available identifiers.
		allSources = {identifier for identifier, metadata in self.endpoints.items() if metadata.isSource}
		allSinks = {identifier for identifier, metadata in self.endpoints.items() if not metadata.isSource}

		for connection in self.connections:

			# Match the identifiers.
			sources = self.getIdentifiers(connection.source, allSources, "source IO")
			sinks: typing.Set[EndpointId] = set()
			for entity in connection.sinks:
				matches = self.getIdentifiers(entity, allSinks, "sink IO")
				entity.assertTrue(condition=bool(matches - sinks),
				                  message=f"This expression does not match any IO sinks.")
				sinks.update(matches)

			# Register the connections.
			for source in sources:
				for sink in sinks:
					try:
						self.addConnection(source=source, sink=sink)
					except AssertionError as e:
						connection.source.error(str(e))

	def __repr__(self) -> str:
		content = []
		for source, metadata in self.endpoints.items():
			content += [f"{source} => {metadata.connections}"]
		return "\n".join(content)
