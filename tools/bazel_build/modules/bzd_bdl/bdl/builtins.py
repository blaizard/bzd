import typing

from bzd.parser.element import Element
from bdl.entities.impl.builtin import Builtin
from bdl.entities.builder import ElementBuilder
from bdl.entities.impl.entity import EntityExpression


def toLiteralSingleValue_(self: Builtin, args: typing.Dict[str, EntityExpression]) -> typing.Optional[str]:
	"""Helper function to return the literal of the only argument 'value' or None if there is no literal."""

	self.assertTrue(len(args) == 1, "There must always be a single argument.")
	self.assertTrue(
	    "value" in args,
	    f"There must always be a single argument named 'value', not '{str(args)}'.",
	)
	if args["value"].isLiteral:
		return args["value"].literal
	return None


# Meta types/functions


class AnyMeta(Builtin):

	def __init__(self) -> None:
		super().__init__(
		    ElementBuilder("builtin").setAttr("name", "Any").setAttr("meta", "1").addConfigValue(name="default",
		                                                                                         literal="0"))


class ListMeta(Builtin):

	def __init__(self) -> None:
		super().__init__(
		    ElementBuilder("builtin").setAttr("name", "list").addConfigValue(name="values...",
		                                                                     symbol="Any").setAttr("meta", "1"))


class ConnectMeta(Builtin):

	def __init__(self) -> None:
		super().__init__(
		    ElementBuilder("builtin").setAttr("name", "connect").addConfigValue(
		        name="source", symbol="Any", contract="mandatory").addConfigValue(name="sinks...",
		                                                                          symbol="Any").setAttr("meta", "1"))


class BindMeta(Builtin):

	def __init__(self) -> None:
		super().__init__(
		    ElementBuilder("builtin").setAttr("name", "bind").addConfigValue(name="symbols...",
		                                                                     symbol="Any").setAttr("meta", "1"))


# Concrete types


class NoneType(Builtin):

	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "None"))


class Void(Builtin):

	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Void"))


class Integer(Builtin):

	def __init__(self) -> None:
		super().__init__(
		    ElementBuilder("builtin").setAttr("name", "Integer").addContract("integer").addConfigValue(name="value",
		                                                                                               literal="0"))

	def toLiteral(self, args: typing.Dict[str, EntityExpression]) -> typing.Optional[str]:
		return toLiteralSingleValue_(self, args)


class Float(Builtin):

	def __init__(self) -> None:
		super().__init__(
		    ElementBuilder("builtin").setAttr("name", "Float").addContract("float").addConfigValue(name="value",
		                                                                                           literal="0"))

	def toLiteral(self, args: typing.Dict[str, EntityExpression]) -> typing.Optional[str]:
		return toLiteralSingleValue_(self, args)


class Boolean(Builtin):

	def __init__(self) -> None:
		super().__init__(
		    ElementBuilder("builtin").setAttr("name", "Boolean").addContract("boolean").addConfigValue(name="value",
		                                                                                               literal="false"))

	def toLiteral(self, args: typing.Dict[str, EntityExpression]) -> typing.Optional[str]:
		return toLiteralSingleValue_(self, args)


class Byte(Builtin):

	def __init__(self) -> None:
		super().__init__(
		    ElementBuilder("builtin").setAttr("name", "Byte").addContract("integer min(0) max(255)").addConfigValue(
		        name="value", literal="0"))

	def toLiteral(self, args: typing.Dict[str, EntityExpression]) -> typing.Optional[str]:
		return toLiteralSingleValue_(self, args)


class String(Builtin):

	def __init__(self) -> None:
		super().__init__(
		    ElementBuilder("builtin").setAttr("name", "String").addContract("string").addConfigValue(name="value",
		                                                                                             literal='""'))

	def toLiteral(self, args: typing.Dict[str, EntityExpression]) -> typing.Optional[str]:
		return toLiteralSingleValue_(self, args)


class Result(Builtin):

	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Result").addConfigType(name="Value", symbol="Any"))


class Async(Builtin):

	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Async").addConfigType(name="Value", symbol="Any"))


class Array(Builtin):

	def __init__(self) -> None:
		super().__init__(
		    ElementBuilder("builtin").setAttr("name", "Array").addConfigType(symbol="Any",
		                                                                     name="Type",
		                                                                     contract="mandatory").addConfigValue(
		                                                                         name="values...", symbol="Any"))


class Vector(Builtin):

	def __init__(self) -> None:
		super().__init__(
		    ElementBuilder("builtin").setAttr("name", "Vector").addConfigType(symbol="Any",
		                                                                      name="Type",
		                                                                      contract="mandatory").addConfigValue(
		                                                                          name="values...", symbol="Any"))


class Callable(Builtin):

	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Callable"))


Builtins = [
    AnyMeta(),
    ListMeta(),
    ConnectMeta(),
    BindMeta(),
    NoneType(),
    Void(),
    Integer(),
    Float(),
    Boolean(),
    Byte(),
    String(),
    Result(),
    Async(),
    Array(),
    Vector(),
    Callable(),
]
