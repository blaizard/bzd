import typing

from tools.bdl.entities.all import Expression
from tools.bdl.generators.cc.types import typeToStr

def valuesToList(entity: Expression) -> typing.List[str]:
	"""Return the values of an expression as a list of string."""

	entity.assertTrue(condition=entity.isRoleValue, message=f"'valuesToList' only applies to entity with role values, not '{entity}'.")

	if entity.isLiteral:
		return [entity.literal]
	elif entity.parametersResolved:
		return [valuesToStr(v) for v in entity.parameters]
	return []

def valuesToStr(entity: Expression,
	registry: typing.Optional[typing.Sequence[str]] = None) -> str:
	"""Convert an expression object into a C++ string.
	Args:
		entity: The Type entity to be converted.
		registry: FQNs that matches a registry entry.
	"""

	entity.assertTrue(condition=entity.isRoleValue, message=f"'valuesToStr' only applies to entity with role values, not '{entity}'.")

	values = []
	if entity.isLiteral:
		values = [entity.literal]
	elif entity.parametersResolved:
		values = [valuesToStr(v) for v in entity.parameters]

	if entity.isType:
		typeStr = typeToStr(entity=entity.typeResolved, values=values)
		return f"{typeStr}{{{', '.join(values)}}}"
	elif len(values) == 1:
		return values[0]
	
	entity.error(message="Multiple values must have a type.")
