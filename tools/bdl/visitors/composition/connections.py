import typing
import dataclasses

from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.entities.impl.entity import EntityExpression
from tools.bdl.entities.impl.fragment.symbol import Symbol
from tools.bdl.entities.impl.types import Category


@dataclasses.dataclass
class OutputMetadata:
	# The maximum history required by this connection.
	history: int = 1


@dataclasses.dataclass
class ConnectionGroup:
	# The type of symbol for this connection group.
	symbol: typing.Optional[Symbol] = None
	# The set of connection object for this group.
	outputs: typing.Dict[Symbol, OutputMetadata] = dataclasses.field(default_factory=dict)


class Connections:

	def __init__(self, symbols: SymbolMap) -> None:
		self.resolver = symbols.makeResolver()
		self.outputs: typing.Set[Symbol] = set()
		self.groups: typing.Dict[Symbol, ConnectionGroup] = {}
		self.recorders: typing.Dict[Symbol, typing.Set[EntityExpression]] = {}

	def addRecorder(self, recorder: Symbol, output: EntityExpression) -> None:
		"""Register a new recorder."""

		if recorder not in self.recorders:
			self.recorders[recorder] = set()
		self.recorders[recorder].add(output)

	def _add(self, input: Symbol, output: Symbol) -> None:
		"""Add a new connection."""

		# Sanity checks.
		input.assertTrue(condition=input != output, message="A connection cannot connect to itself.")

		# Check const correctness.
		inputEntity = input.getEntityResolved(resolver=self.resolver)
		assert isinstance(inputEntity, EntityExpression)
		inputEntity.assertTrue(condition=not inputEntity.symbol.const,
		                       message="A connection sender must not be marked as const.")
		outputEntity = output.getEntityResolved(resolver=self.resolver)
		assert isinstance(outputEntity, EntityExpression)
		outputEntity.assertTrue(condition=outputEntity.symbol.const,
		                        message="A connection receiver must be marked as const.")

		alreadyInserted = input in self.groups and output in self.groups[input].outputs
		input.assertTrue(condition=not alreadyInserted,
		                 message=f"Connection between '{input}' and '{output}' is defined multiple times.")
		input.assertTrue(condition=input not in self.outputs,
		                 message=f"'{input}' has already been defined as an output.")
		if input not in self.groups:
			inputEntityType = input.getEntityUnderlyingTypeResolved(resolver=self.resolver)
			assert hasattr(inputEntityType, "symbol")
			self.groups[input] = ConnectionGroup(symbol=inputEntityType.symbol)
		self.groups[input].outputs[output] = OutputMetadata()
		self.outputs.add(output)

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

		self._add(input=input.symbol, output=output.symbol)

	def close(self) -> None:
		# List of all signal ids.
		signals = set(self.groups.keys())
		for recorder, entities in self.recorders.items():

			matches: typing.Set[Symbol] = set()
			for entity in entities:
				# If the entity contains a regular expression.
				if entity.isRegexpr:
					matches.update(entity.regexpr.match(signals))
				# Else it should point to a signal input.
				else:
					entity.assertTrue(condition=entity.isSymbol, message="Must be a regexpr of a symbol.")
					entity.assertTrue(condition=entity.isLValue, message="Must be a reference to another object.")
					entity.assertTrue(condition=entity.symbol in signals,
					                  message="A recorder must be connected to a IO generator.")
					matches.add(entity.symbol)

			# Register the connections.
			for input in matches:
				self._add(input=input, output=recorder)

	def __repr__(self) -> str:
		content = []
		for input, connection in self.groups.items():
			content += [f"{input} => {connection.outputs.keys()}"]
		return "\n".join(content)
