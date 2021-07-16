from bzd.parser.element import Element
from tools.bdl.entities.impl.builtin import Builtin
from tools.bdl.entities.builder import ElementBuilder

#BuiltinVoid = Builtin(ElementBuilder("builtin").addAttr("name", "Void").addAttr("validation", '{}'))
BuiltinVoid = Builtin(ElementBuilder("builtin").addAttr("name", "Void"))
#BuiltinInteger = Builtin(ElementBuilder("builtin").addAttr("name", "Integer").addAttr("validation", '{"0": "integer"}').addContract("integer"))
BuiltinInteger = Builtin(ElementBuilder("builtin").addAttr("name", "Integer").addContract("integer"))
#BuiltinFloat = Builtin(ElementBuilder("builtin").addAttr("name", "Float").addAttr("validation", '{"0": "float"}'))
BuiltinFloat = Builtin(ElementBuilder("builtin").addAttr("name", "Float").addContract("float"))
#BuiltinBoolean = Builtin(ElementBuilder("builtin").addAttr("name", "Boolean").addAttr("validation", '{"0": "boolean"}'))
BuiltinBoolean = Builtin(ElementBuilder("builtin").addAttr("name", "Boolean").addContract("boolean"))
BuiltinResult = Builtin(
	ElementBuilder("builtin").addAttr("name", "Result").addAttr("validation_template",
	'{"0": "mandatory", "1": "mandatory"}'))
#BuiltinResult = Builtin(
#	ElementBuilder("builtin").addAttr("name", "Result").addConfig(
#		ElementBuilder("expression").addContract("template").addContract("mandatory"),
#		ElementBuilder("expression").addContract("template").addContract("mandatory")
#	))
BuiltinList = Builtin(
	ElementBuilder("builtin").addAttr("name", "List").addAttr("validation_template", '{"0": "mandatory"}'))
BuiltinCallable = Builtin(ElementBuilder("builtin").addAttr("name", "Callable").addAttr("validation", '{}'))

Builtins = [BuiltinVoid, BuiltinInteger, BuiltinFloat, BuiltinBoolean, BuiltinResult, BuiltinList, BuiltinCallable]
