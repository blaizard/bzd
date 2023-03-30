import typing

from tools.bdl.generators.cc.symbol import symbolToStr
from tools.bdl.generators.cc.fqn import fqnToNameStr
from tools.bdl.generators.cc.comments import commentParametersResolvedToStr
from tools.bdl.entities.impl.fragment.parameters_resolved import ParametersResolvedItem
from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.entities.impl.types import Category
from tools.bdl.entities.impl.expression import Expression


def valueToStr(item: ParametersResolvedItem,
               symbols: typing.Optional[SymbolMap] = None,
               registry: typing.Optional[typing.Sequence[str]] = None,
               includeComment: bool = True) -> str:
	"""Convert an item object into a C++ string.
	Args:
		item: The item to be converted.
		symbols: The symbols map to be used.
		registry: FQNs that matches a registry entry.
		includeComment; Whether or not comment should be prepended to the value.
	"""

	item.assertTrue(condition=item.param.isRoleValue,
	                message=f"'valueToStr' only applies to item with role values, not '{item}'.")

	def bindTypeAndValue(values: typing.List[str]) -> str:
		"""Associate a value with its type."""

		paramTypeStr = symbolToStr(item.param.symbol, referenceForInterface=True,
		                           values=values) if item.param.isSymbol else ""
		paramValues = values if paramTypeStr == "" else [f"{paramTypeStr}{{{', '.join(values)}}}"]

		# If the types (param and expected) are the same, return already.
		if item.sameType:
			return ", ".join(paramValues)

		expectedTypeStr = symbolToStr(item.expected.symbol, referenceForInterface=True,
		                              values=paramValues) if item.expected.isSymbol else ""
		return ", ".join(paramValues) if expectedTypeStr == "" else f"{expectedTypeStr}{{{', '.join(paramValues)}}}"

	if item.param.isLiteral:
		value = bindTypeAndValue([item.param.literal])  # type: ignore
	elif item.isLValue:
		fqn = item.param.underlyingValueFQN
		assert fqn is not None
		value = fqnToNameStr(fqn)
		if registry is not None:
			# Get the value from the registry instead of directly.
			assert fqn in registry, f"The fqn '{fqn}' is not present in the registry."
			value = f"registry.{value}_.get()"
		if symbols is not None:
			# Cast values to there underlying interface type.
			expectedInterface = item.expected.underlyingInterfaceFQN or item.expected.underlyingTypeFQN
			assert expectedInterface is not None
			if symbols.getEntityResolved(expectedInterface).value.category == Category.interface:
				value = f"bzd::Interface<\"{expectedInterface}\">::cast({value})"
	elif item.isRValue:
		assert isinstance(item.param, Expression)
		values = [valueToStr(item=i, symbols=symbols, registry=registry) for i in item.param.parametersResolved]
		value = bindTypeAndValue(values)
	else:
		item.error(message="Type not supported, should never happen.")

	if includeComment:
		return f"{commentParametersResolvedToStr(item)}{value}"
	return value
