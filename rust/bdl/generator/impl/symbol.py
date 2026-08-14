import typing


from bdl.entities.impl.fragment.symbol import Symbol, Visitor
from bdl.entities.impl.fragment.parameters_resolved import ParametersResolved
from bdl.entities.impl.fragment.fqn import FQN
from bdl.entities.impl.types import Category

from rust.bdl.generator.impl.builtins import builtins


def fqnToCapitalized(fqn: str) -> str:
	return "".join([part[:1].upper() + part[1:] for part in FQN.toNamespace(fqn)])


class _VisitorRustSymbol(Visitor):
	"""Visitor to print a Rust type."""

	def __init__(
		self,
		symbol: Symbol,
		reference: bool,
		nonConst: bool = False,
		values: typing.Optional[typing.Sequence[str]] = None,
	) -> None:
		self.reference = reference
		self.nonConst = nonConst
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
		for fqn in symbol.kinds:
			if fqn in builtins:
				if callable(builtins[fqn].toType):
					output, nested = builtins[fqn].toType(symbol, nested, useReference, self.values)
				else:
					output = builtins[fqn].toType
			else:
				output = fqnToCapitalized(fqn=fqn)
			outputList.append(output)
		output = "::".join(outputList)

		# Special value, which defines an empty type.
		if output == "":
			return ""

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

		# Apply const if needed. In Rust, a const type is a shared reference.
		# Types that already are references are left untouched.
		# Scalar value types are trivially copyable and are passed by value instead.
		isValueType = symbol.kinds[0] in builtins and getattr(builtins[symbol.kinds[0]], "isCopy", False)
		if symbol.const and self.isTopLevel and not self.nonConst and not output.startswith("&") and not isValueType:
			output = "&" + output

		return output


def symbolRustToStr(
	symbol: typing.Optional[Symbol],
	reference: bool = False,
	nonConst: bool = False,
) -> str:
	if symbol is None:
		return "()"
	return _VisitorRustSymbol(symbol=symbol, reference=reference, nonConst=nonConst).result
