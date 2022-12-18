import typing

from bzd.parser.element import Element
from tools.bdl.entities.impl.builtin import Builtin
from tools.bdl.entities.builder import ElementBuilder

class Void(Builtin):
	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Void"))

class Any(Builtin):
	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Any"))

class Integer(Builtin):
	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Integer").addContract("integer"))

	def toLiteral(self, args: typing.Any) -> typing.Optional[str]:
		if len(args) == 0:
			return "0"
		return str(args["0"])

class Float(Builtin):
	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Float").addContract("float").setAttr("parents", "Integer"))

	def toLiteral(self, args: typing.Any) -> typing.Optional[str]:
		if len(args) == 0:
			return "0"
		return str(args["0"])

class Boolean(Builtin):
	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Boolean").addContract("boolean"))

	def toLiteral(self, args: typing.Any) -> typing.Optional[str]:
		if len(args) == 0:
			return "false"
		return "true" if bool(args["0"]) else "false"

class Byte(Builtin):
	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Byte").addContract("integer min(0) max(255)"))

	def toLiteral(self, args: typing.Any) -> typing.Optional[str]:
		if len(args) == 0:
			return "0"
		return str(args["0"])

class String(Builtin):
	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "String").addContract("string"))

	def toLiteral(self, args: typing.Any) -> typing.Optional[str]:
		return str(args["0"])

class Result(Builtin):
	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Result").addConfigType(name="Value", kind="Any").addConfigType(name="Error", kind="Any"))

class Async(Builtin):
	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Async").addConfigType(name="Value", kind="Any").addConfigType(name="Error", kind="Any"))

class Span(Builtin):
	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Span").addConfigType(kind="Any", name="Type", contract="mandatory"))

class Vector(Builtin):
	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Vector").addConfigType(kind="Any", name="Type", contract="mandatory").addConfigValue(name="...", kind="Any"))

class Callable(Builtin):
	def __init__(self) -> None:
		super().__init__(ElementBuilder("builtin").setAttr("name", "Callable"))

Builtins = [
	Void(), Any(), Integer(), Float(), Boolean(), Byte(), String(), Span(), Result(), Async(), Vector(), Callable()
]
