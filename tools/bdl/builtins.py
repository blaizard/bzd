from bzd.parser.element import Element
from tools.bdl.entities.impl.builtin import Builtin
from tools.bdl.entities.builder import ElementBuilder

BuiltinVoid = Builtin(ElementBuilder("builtin").setAttr("name", "Void"))
BuiltinInteger = Builtin(ElementBuilder("builtin").setAttr("name", "Integer").addContract("integer"))
BuiltinFloat = Builtin(ElementBuilder("builtin").setAttr("name", "Float").addContract("float"))
BuiltinBoolean = Builtin(ElementBuilder("builtin").setAttr("name", "Boolean").addContract("boolean"))
BuiltinResult = Builtin(
	ElementBuilder("builtin").setAttr("name", "Result").addConfig(kind="Any",
	contract="template mandatory type").addConfig(kind="Any", contract="template mandatory type"))
BuiltinList = Builtin(
	ElementBuilder("builtin").setAttr("name", "List").addConfig(kind="Any",
	contract="template mandatory type").addConfig(name="...", kind="Any"))
BuiltinCallable = Builtin(ElementBuilder("builtin").setAttr("name", "Callable"))

Builtins = [BuiltinVoid, BuiltinInteger, BuiltinFloat, BuiltinBoolean, BuiltinResult, BuiltinList, BuiltinCallable]
