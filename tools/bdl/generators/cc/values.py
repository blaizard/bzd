import typing

from tools.bdl.entities.all import Expression
from tools.bdl.generators.cc.types import typeToStr

def valueToStr(entity: Expression,
	registry: typing.Optional[typing.Sequence[str]] = None) -> str:
	"""
	Convert an expression object into a C++ string.
	Args:
		entity: The Type entity to be converted.
		registry: FQNs that matches a registry entry.
	"""

	entity.assertTrue(condition=entity.isRoleValue, message=f"'valueToStr' only applies to entity with role values, not '{entity}'.")

	value = ""
	if entity.isLiteral:
		value = entity.literal
	elif entity.parametersResolved:
		value = ", ".join([valueToStr(v) for v in entity.parameters])

	if entity.isType:
		value = f"{typeToStr(entity=entity.typeResolved)}{{{value}}}"

	return value
