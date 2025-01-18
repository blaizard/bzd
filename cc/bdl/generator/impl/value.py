import typing

from bdl.entities.impl.fragment.parameters_resolved import ParametersResolvedItem
from bdl.visitors.symbol_map import SymbolMap
from bdl.entities.impl.types import Category
from bdl.entities.impl.expression import Expression

from cc.bdl.generator.impl.symbol import symbolToStr
from cc.bdl.generator.impl.fqn import fqnToNameStr
from cc.bdl.generator.impl.comments import commentParametersResolvedToStr


def valueToStr(
    item: ParametersResolvedItem,
    symbols: typing.Optional[SymbolMap] = None,
    registry: bool = False,
    includeComment: bool = True,
) -> str:
	"""Convert an item object into a C++ string.
    Args:
            item: The item to be converted.
            symbols: The symbols map to be used.
            registry: True, if the value is expected to be from the registry.
            includeComment; Whether or not comment should be prepended to the value.
    """

	item.assertTrue(
	    condition=item.param.isRoleValue,
	    message=f"'valueToStr' only applies to item with role values, not '{item}'.",
	)

	def bindTypeAndValue(values: typing.List[str]) -> str:
		"""Associate a value with its type."""

		paramTypeStr = (symbolToStr(item.param.symbol, values=values) if item.param.isSymbol else "")
		paramValues = (values if paramTypeStr == "" else [f"{paramTypeStr}{{{', '.join(values)}}}"])

		# If the types (param and expected) are the same, return already.
		if item.sameType:
			return ", ".join(paramValues)

		expectedTypeStr = (symbolToStr(item.expected.symbol, values=paramValues) if item.expected.isSymbol else "")
		return (", ".join(paramValues) if expectedTypeStr == "" else f"{expectedTypeStr}{{{', '.join(paramValues)}}}")

	if item.param.isLiteral:
		value = bindTypeAndValue([item.param.literal])  # type: ignore
	elif item.isLValue:
		fqn = item.param.underlyingValueFQN
		assert fqn is not None
		value = fqnToNameStr(fqn)
		if registry:
			value = f"registry.{value}_.get()"
		if symbols is not None:
			# Cast values to there underlying interface type.
			expectedInterface = (item.expected.underlyingInterfaceFQN or item.expected.underlyingTypeFQN)
			assert expectedInterface is not None
			if (symbols.getEntityResolved(expectedInterface).value.category == Category.interface):
				value = f'bzd::Interface<"{expectedInterface}">::cast({value})'
	elif item.isRValue:
		assert isinstance(item.param, Expression)
		values = [valueToStr(item=i, symbols=symbols, registry=registry) for i in item.param.parametersResolved]
		value = bindTypeAndValue(values)
	else:
		item.error(message="Type not supported, should never happen.")

	if includeComment:
		return f"{commentParametersResolvedToStr(item)}{value}"
	return value
