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

	values = []
	if entity.isLiteral:
		values = [entity.literal]
	elif entity.parametersResolved:
		values = [valueToStr(v) for v in entity.parameters]

	if entity.isType:
		typeStr = typeToStr(entity=entity.typeResolved, values=values)
		return f"{typeStr}{{{', '.join(values)}}}"
	elif len(values) == 1:
		return values[0]
	
	entity.error(message="Multiple values must have a type.")
