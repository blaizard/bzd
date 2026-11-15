from bzd.parser.element import Element
from bzd.parser.error import Error

from bdl.entities.impl.builtin import Builtin as _Builtin
from bdl.entities.impl.enum import Enum as _Enum
from bdl.entities.impl.enum import EnumValue as _EnumValue
from bdl.entities.impl.expression import Expression as _Expression
from bdl.entities.impl.extern import Extern as _Extern
from bdl.entities.impl.method import Method as _Method
from bdl.entities.impl.namespace import Namespace as _Namespace
from bdl.entities.impl.nested import Nested as _Nested
from bdl.entities.impl.preset import Preset as _Preset
from bdl.entities.impl.reference import Reference as _Reference
from bdl.entities.impl.use import Use as _Use
from bdl.entities.impl.using import Using as _Using

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
Preset = _Preset

EntityType = (
	Expression | Nested | Method | Using | Enum | EnumValue | Extern | Namespace | Use | Builtin | Reference | Preset
)
SymbolType = Expression | Nested | Method | Using | Enum | Extern

CATEGORY_TO_ENTITY: dict[str, type[EntityType]] = {
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
	"preset": Preset,
}


def elementToEntity(
	element: Element,
	extension: dict[str, type[EntityType]] | None = None,
) -> EntityType:
	"""
	Instantiate an entity from an element.
	"""

	Error.assertHasAttr(element=element, attr="category")
	category = element.getAttr("category").value

	if extension and category in extension:
		return extension[category](element=element)

	if category not in CATEGORY_TO_ENTITY:
		Error.handleFromElement(element=element, message=f"Unexpected element category: {category}")

	return CATEGORY_TO_ENTITY[category](element=element)
