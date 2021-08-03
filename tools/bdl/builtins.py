from bzd.parser.element import Element
from tools.bdl.entities.impl.builtin import Builtin
from tools.bdl.entities.builder import ElementBuilder

BuiltinVoid = Builtin(ElementBuilder("builtin").addAttr("name", "Void"))
BuiltinInteger = Builtin(
	ElementBuilder("builtin").addAttr("name", "Integer").addAttr("literal", "0").addContract("integer"))
BuiltinFloat = Builtin(ElementBuilder("builtin").addAttr("name", "Float").addAttr("literal", "0").addContract("float"))
BuiltinBoolean = Builtin(
	ElementBuilder("builtin").addAttr("name", "Boolean").addAttr("literal", "false").addContract("boolean"))
BuiltinResult = Builtin(
	ElementBuilder("builtin").addAttr("name", "Result").addConfig(kind="Any",
	contract="template mandatory type").addConfig(kind="Any", contract="template mandatory type"))
BuiltinList = Builtin(
	ElementBuilder("builtin").addAttr("name", "List").addConfig(kind="Any", contract="template mandatory"))
BuiltinCallable = Builtin(ElementBuilder("builtin").addAttr("name", "Callable"))

Builtins = [BuiltinVoid, BuiltinInteger, BuiltinFloat, BuiltinBoolean, BuiltinResult, BuiltinList, BuiltinCallable]
