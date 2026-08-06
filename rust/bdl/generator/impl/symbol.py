import typing


from bdl.entities.impl.fragment.symbol import Symbol, Visitor
from bdl.entities.impl.fragment.parameters_resolved import ParametersResolved
from bdl.entities.impl.fragment.fqn import FQN
from bdl.entities.impl.types import Category

from rust.bdl.generator.impl.builtins import builtins


class _VisitorRustSymbol(Visitor):
	"""Visitor to print a Rust type."""

	def __init__(
		self,
		symbol: Symbol,
		reference: bool,
		values: typing.Optional[typing.Sequence[str]] = None,
	) -> None:
		self.reference = reference
		self.values_ = values
		self.symbol = symbol
		super().__init__(symbol=symbol)

	@property
	def values(self) -> typing.Optional[typing.Sequence[str]]:
		if self.isTopLevel:
			return self.values_
		return None

	def visitValue(self, value: str, comment: typing.Optional[str]) -> str:
		self.symbol.error(message="No values are allowed within a type.")
		return ""

	def visitSymbol(self, symbol: Symbol, nested: typing.List[str], parameters: ParametersResolved) -> str:
		"""Called when an element needs to be formatted."""

		# Whether this type should be a reference or not.
		useReference = (
			self.reference
			if self.isTopLevel
			else (symbol.category == Category.interface or symbol.category == Category.component)
		)

		outputList: typing.List[str] = []
		output: str
		for index, fqn in enumerate(symbol.kinds):
			if fqn in builtins:
				if callable(builtins[fqn].toType):
					output, nested = builtins[fqn].toType(symbol, nested, useReference, self.values)
				else:
					output = builtins[fqn].toType
			else:
				namespace = FQN.toNamespace(fqn)
				if index == 0:
					output = "::".join(namespace)
				else:
					output = namespace[-1]
			outputList.append(output)
		output = "::".join(outputList)

		# Apply the nested template if any.
		if nested:
			output += "<{}>".format(", ".join(nested))

		# Apply the reference if any.
		# TODO: emit &dyn Trait / &T for interface/component references.
		if useReference:
			if symbol.category == Category.interface or symbol.category == Category.component:
				pass
			else:
				output += "&"

		return output


def symbolRustToStr(symbol: typing.Optional[Symbol], reference: bool = False) -> str:
	if symbol is None:
		return "()"
	return _VisitorRustSymbol(symbol=symbol, reference=reference).result
