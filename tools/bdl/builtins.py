from bzd.parser.element import Element
from tools.bdl.entities.impl.builtin import Builtin
from tools.bdl.entities.builder import ElementBuilder

BuiltinVoid = Builtin(ElementBuilder("builtin").addAttr("name", "Void"))
BuiltinInteger = Builtin(ElementBuilder("builtin").addAttr("name", "Integer").addContract("integer"))
BuiltinFloat = Builtin(ElementBuilder("builtin").addAttr("name", "Float").addContract("float"))
BuiltinBoolean = Builtin(ElementBuilder("builtin").addAttr("name", "Boolean").addContract("boolean"))
BuiltinResult = Builtin(
	ElementBuilder("builtin").addAttr("name", "Result").addConfig(kind="Any",
	contract="template mandatory").addConfig(kind="Any", contract="template mandatory"))
BuiltinList = Builtin(
	ElementBuilder("builtin").addAttr("name", "List").addConfig(kind="Any", contract="template mandatory"))
BuiltinCallable = Builtin(ElementBuilder("builtin").addAttr("name", "Callable"))

Builtins = [BuiltinVoid, BuiltinInteger, BuiltinFloat, BuiltinBoolean, BuiltinResult, BuiltinList, BuiltinCallable]
