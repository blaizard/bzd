import typing

from tools.bdl.generators.cc.types import typeToStr
from tools.bdl.generators.cc.fqn import fqnToNameStr
from tools.bdl.generators.cc.comments import commentParametersResolvedToStr
from tools.bdl.entities.impl.fragment.parameters_resolved import ParametersResolvedItem
from tools.bdl.visitors.symbol_map import SymbolMap

def valueToStr(item: ParametersResolvedItem, symbols: typing.Optional[SymbolMap] = None, registry: typing.Optional[typing.Sequence[str]] = None) -> str:
	"""Convert an item object into a C++ string.
	Args:
		item: The item to be converted.
		symbols: The symbols map to be used.
		registry: FQNs that matches a registry entry.
	"""

	item.assertTrue(condition=item.param.isRoleValue, message=f"'valueToStr' only applies to item with role values, not '{item}'.")

	if item.param.isLiteral:
		value = f"{typeToStr(item.type)}{{{item.param.literal}}}"
	elif item.isLValue:
		fqn = item.param.underlyingValueFQN
		value = fqnToNameStr(fqn)
		if registry is not None:
			# Get the value from the registry instead of directly.
			assert fqn in registry, f"The fqn '{fqn}' is not present in the registry."
			value = f"registry.{value}_.get()"
		if symbols is not None:
			# Cast values to there underlying interface type.
			expectedType = item.expected.underlyingTypeFQN
			if symbols.getEntityResolved(expectedType).value.category == "interface":
				value = f"bzd::Interface<\"{expectedType}\">::cast({value})"
	elif item.isRValue:
		values = [valueToStr(item=i, symbols=symbols, registry=registry) for i in item.param.parametersResolved]
		typeStr = typeToStr(item.type, referenceForInterface=True, values=values)
		value = f"{typeStr}{{{', '.join(values)}}}"
	else:
		item.error(message="Type not supported, should never happen.")

	# Concatenate the comment with the value.
	return f"{commentParametersResolvedToStr(item)}{value}"
