import typing

from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.symbol import Symbol, Visitor
from tools.bdl.entities.impl.fragment.parameters_resolved import ParametersResolved
from tools.bdl.entities.impl.fragment.fqn import FQN
from tools.bdl.generators.cc.comments import commentEmbeddedToStr
from tools.bdl.generators.cc.fqn import fqnToNameStr
from tools.bdl.generators.cc.builtins import builtins
from tools.bdl.entities.impl.types import Category


class _VisitorSymbol(Visitor):
	"""Visitor to print a type."""

	def __init__(self, symbol: Symbol, namespaceToFQN: typing.Optional[typing.Callable[[typing.List[str]], str]],
	             reference: bool, definition: bool, nonConst: bool, referenceForInterface: bool,
	             values: typing.Optional[typing.Sequence[str]]) -> None:
		self.namespaceToFQN = namespaceToFQN
		self.reference = reference
		self.definition = definition
		self.nonConst = nonConst
		self.referenceForInterface = referenceForInterface
		self.values_ = values
		self.symbol = symbol
		super().__init__(symbol=symbol)

	@property
	def isNamespaceToFQN(self) -> bool:
		return self.isTopLevel and self.namespaceToFQN is not None

	@property
	def values(self) -> typing.Optional[typing.Sequence[str]]:
		if self.isTopLevel:
			return self.values_
		return None

	def visitValue(self, value: str, comment: typing.Optional[str]) -> str:
		self.symbol.error(message="No values are allowed within a type.")
		return ""

	def visitSymbol(self, symbol: Symbol, nested: typing.List[str], parameters: ParametersResolved) -> str:
		"""
		Called when an element needs to be formatted.
		"""

		# Whether this type should be a reference or not.
		useReference = self.isTopLevel and self.reference
		useReference |= self.referenceForInterface and symbol.category == Category.interface

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

		if self.isTopLevel:
			if self.definition:
				if symbol.underlyingTypeFQN in builtins and builtins[symbol.underlyingTypeFQN].constexpr:
					output = "constexpr " + output

		# Apply the reference if any.
		if useReference:
			output += "&"

		# Apply const if needed.
		if symbol.const and (not self.isTopLevel or not self.nonConst):
			output = "const " + output

		return output


def symbolToStr(symbol: typing.Optional[Symbol],
                adapter: bool = False,
                reference: bool = False,
                definition: bool = False,
                nonConst: bool = False,
                referenceForInterface: bool = False,
                values: typing.Optional[typing.Sequence[str]] = None,
                registry: typing.Optional[typing.Sequence[str]] = None) -> str:
	"""
	Convert a type object into a C++ string.
	Args:
		symbol: The symbol to be converted.
		adapter: If the type should be converted into its adapter.
		reference: If the symbol is passed as reference.
		definition: The type is used for a variable definition.
		registry: FQNs that matches a registry entry.
	"""

	if symbol is None:
		return "void"

	namespaceToFQN: typing.Optional[typing.Callable[[typing.List[str]], str]] = None
	if adapter:

		def namespaceToFQN(namespace: typing.List[str]) -> str:
			return "::".join(namespace[0:-1] + ["adapter"] + namespace[-1:])

	# If the type is part of the registry
	elif registry and symbol.isThis:

		def namespaceToFQN(namespace: typing.List[str]) -> str:
			fqn = FQN.fromNamespace(namespace=namespace)
			return "registry.{}_.get()".format(fqnToNameStr(fqn))

	# Otherwise it must be a normal type
	return _VisitorSymbol(symbol=symbol,
	                      namespaceToFQN=namespaceToFQN,
	                      reference=reference,
	                      definition=definition,
	                      nonConst=nonConst,
	                      referenceForInterface=referenceForInterface,
	                      values=values).result
