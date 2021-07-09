import typing

from bzd.parser.error import Error
from bzd.parser.element import Element

from tools.bdl.entities.impl.builtin import Builtin as _Builtin
from tools.bdl.entities.impl.enum import Enum as _Enum
from tools.bdl.entities.impl.method import Method as _Method
from tools.bdl.entities.impl.namespace import Namespace as _Namespace
from tools.bdl.entities.impl.nested import Nested as _Nested
from tools.bdl.entities.impl.use import Use as _Use
from tools.bdl.entities.impl.using import Using as _Using
from tools.bdl.entities.impl.expression import Expression as _Expression

# Implicitly export all types
Builtin = _Builtin
Enum = _Enum
Method = _Method
Namespace = _Namespace
Nested = _Nested
Use = _Use
Using = _Using
Expression = _Expression

EntityType = typing.Union[Expression, Nested, Method, Using, Enum, Namespace, Use, Builtin]
SymbolType = typing.Union[Expression, Nested, Method, Using, Enum]


def elementToEntity(element: Element) -> EntityType:
	"""
	Instantiate an entity from an element.
	"""

	categoryToEntity: typing.Dict[str, typing.Type[EntityType]] = {
		"nested": Nested,
		"builtin": Builtin,
		"expression": Expression,
		"method": Method,
		"using": Using,
		"enum": Enum,
		"namespace": Namespace,
		"use": Use
	}

	Error.assertHasAttr(element=element, attr="category")
	category = element.getAttr("category").value

	if category not in categoryToEntity:
		Error.handleFromElement(element=element, message="Unexpected element category: {}".format(category))

	return categoryToEntity[category](element=element)
