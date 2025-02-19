import typing

from bzd.parser.error import Error
from bzd.parser.element import Element

from bdl.entities.impl.builtin import Builtin as _Builtin
from bdl.entities.impl.enum import Enum as _Enum, EnumValue as _EnumValue
from bdl.entities.impl.method import Method as _Method
from bdl.entities.impl.namespace import Namespace as _Namespace
from bdl.entities.impl.nested import Nested as _Nested
from bdl.entities.impl.use import Use as _Use
from bdl.entities.impl.using import Using as _Using
from bdl.entities.impl.expression import Expression as _Expression
from bdl.entities.impl.extern import Extern as _Extern
from bdl.entities.impl.reference import Reference as _Reference

# Implicitly export all types
Builtin = _Builtin
Enum = _Enum
EnumValue = _EnumValue
Method = _Method
Namespace = _Namespace
Nested = _Nested
Use = _Use
Using = _Using
Expression = _Expression
Extern = _Extern
Reference = _Reference

EntityType = typing.Union[Expression, Nested, Method, Using, Enum, EnumValue, Extern, Namespace, Use, Builtin,
                          Reference]
SymbolType = typing.Union[Expression, Nested, Method, Using, Enum, Extern]

CATEGORY_TO_ENTITY: typing.Dict[str, typing.Type[EntityType]] = {
    "struct": Nested,
    "interface": Nested,
    "component": Nested,
    "composition": Nested,
    "builtin": Builtin,
    "expression": Expression,
    "method": Method,
    "using": Using,
    "enum": Enum,
    "enumValue": EnumValue,
    "namespace": Namespace,
    "use": Use,
    "reference": Reference,
    "extern": Extern,
}


def elementToEntity(
    element: Element,
    extension: typing.Optional[typing.Dict[str, typing.Type[EntityType]]] = None,
) -> EntityType:
	"""
    Instantiate an entity from an element.
    """

	Error.assertHasAttr(element=element, attr="category")
	category = element.getAttr("category").value

	if extension and category in extension:
		return extension[category](element=element)

	if category not in CATEGORY_TO_ENTITY:
		Error.handleFromElement(element=element, message="Unexpected element category: {}".format(category))

	return CATEGORY_TO_ENTITY[category](element=element)
