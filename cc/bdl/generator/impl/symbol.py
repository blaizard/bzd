import typing

from bdl.entities.impl.fragment.fqn import FQN
from bdl.entities.impl.fragment.parameters_resolved import ParametersResolved
from bdl.entities.impl.fragment.symbol import Symbol, Visitor
from bdl.entities.impl.types import Category
from cc.bdl.generator.impl.builtins import builtins
from cc.bdl.generator.impl.fqn import fqnToNameStr


class _VisitorSymbol(Visitor):
	"""Visitor to print a type."""

	def __init__(
		self,
		symbol: Symbol,
		namespaceToFQN: typing.Callable[[list[str]], str] | None,
		reference: bool,
		definition: bool,
		nonConst: bool,
		values: typing.Sequence[str] | None,
	) -> None:
		self.namespaceToFQN = namespaceToFQN
		self.reference = reference
		self.definition = definition
		self.nonConst = nonConst
		self.values_ = values
		self.symbol = symbol
		super().__init__(symbol=symbol)

	@property
	def isNamespaceToFQN(self) -> bool:
		return self.isTopLevel and self.namespaceToFQN is not None

	@property
	def values(self) -> typing.Sequence[str] | None:
		if self.isTopLevel:
			return self.values_
		return None

	def visitValue(self, value: str, comment: str | None) -> str:
		self.symbol.error(message="No values are allowed within a type.")
		return ""

	def visitSymbol(self, symbol: Symbol, nested: list[str], parameters: ParametersResolved) -> str:
		"""Called when an element needs to be formatted."""

		# Whether this type should be a reference or not.
		useReference = (
			self.reference
			if self.isTopLevel
			else (symbol.category == Category.interface or symbol.category == Category.component)
		)

		outputList: list[str] = []
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
					if self.isNamespaceToFQN:
						assert self.namespaceToFQN is not None
						output = self.namespaceToFQN(namespace)
					else:
						output = "::".join(namespace)
				else:
					output = namespace[-1]
			outputList.append(output)
		output = ".".join(outputList)

		# Special value, which defines an empty type.
		if output == "":
			return ""

		# Apply the nested template if any.
		if nested:
			output += "<{}>".format(", ".join(nested))

		if (
			self.isTopLevel
			and self.definition
			and symbol.underlyingTypeFQN in builtins
			and builtins[symbol.underlyingTypeFQN].constexpr
		):
			output = "constexpr " + output

		# Apply the reference if any.
		if useReference:
			output += "&"

		# Apply const if needed.
		if symbol.const and (not self.isTopLevel or not self.nonConst):
			output = "const " + output

		return output


def symbolToStr(
	symbol: Symbol | None,
	adapter: bool = False,
	reference: bool = False,
	definition: bool = False,
	nonConst: bool = False,
	values: typing.Sequence[str] | None = None,
	registry: bool = False,
) -> str:
	"""
	Convert a type object into a C++ string.
	Args:
	        symbol: The symbol to be converted.
	        adapter: If the type should be converted into its adapter.
	        reference: If the symbol is passed as reference.
	        definition: The type is used for a variable definition.
	        registry: True if the entry is expected to be from the registry (for members only).
	"""

	if symbol is None:
		return "void"

	namespaceToFQN: typing.Callable[[list[str]], str] | None = None
	if adapter:

		def namespaceToFQN(namespace: list[str]) -> str:
			return "::".join(namespace[0:-1] + ["adapter"] + namespace[-1:])

	# If the type is part of the registry
	elif registry and symbol.isThis:

		def namespaceToFQN(namespace: list[str]) -> str:
			fqn = FQN.fromNamespace(namespace=namespace)
			return f"registry.{fqnToNameStr(fqn)}_.get()"

	# Otherwise it must be a normal type
	return _VisitorSymbol(
		symbol=symbol,
		namespaceToFQN=namespaceToFQN,
		reference=reference,
		definition=definition,
		nonConst=nonConst,
		values=values,
	).result
