from bzd.parser.element import Element
from tools.bdl.entities.impl.builtin import Builtin
from tools.bdl.entities.builder import ElementBuilder

BuiltinVoid = Builtin(ElementBuilder("builtin").setAttr("name", "Void"))
BuiltinAny = Builtin(ElementBuilder("builtin").setAttr("name", "Any"))
BuiltinInteger = Builtin(ElementBuilder("builtin").setAttr("name", "Integer").addContract("integer"))
BuiltinFloat = Builtin(ElementBuilder("builtin").setAttr("name", "Float").addContract("float"))
BuiltinBoolean = Builtin(ElementBuilder("builtin").setAttr("name", "Boolean").addContract("boolean"))
BuiltinByte = Builtin(ElementBuilder("builtin").setAttr("name", "Byte").addContract("integer min(0) max(255)"))
BuiltinString = Builtin(ElementBuilder("builtin").setAttr("name", "String").addContract("string"))
BuiltinResult = Builtin(
	ElementBuilder("builtin").setAttr("name", "Result").addConfig(kind="Any",
	contract="template type").addConfig(kind="Any", contract="template type"))
BuiltinAsync = Builtin(
	ElementBuilder("builtin").setAttr("name", "Async").addConfig(kind="Any",
	contract="template type").addConfig(kind="Any", contract="template type"))
BuiltinSpan = Builtin(
	ElementBuilder("builtin").setAttr("name", "Span").addConfig(kind="Any", contract="template mandatory type"))
BuiltinVector = Builtin(
	ElementBuilder("builtin").setAttr("name", "Vector").addConfig(kind="Any",
	contract="template mandatory type").addConfig(name="...", kind="Any"))
BuiltinCallable = Builtin(ElementBuilder("builtin").setAttr("name", "Callable"))

Builtins = [
	BuiltinVoid, BuiltinAny, BuiltinInteger, BuiltinFloat, BuiltinBoolean, BuiltinByte, BuiltinString, BuiltinSpan,
	BuiltinResult, BuiltinAsync, BuiltinVector, BuiltinCallable
]
