from bzd.parser.element import Element
from tools.bdl.entities.impl.builtin import Builtin
from tools.bdl.entities.builder import ElementBuilder

BuiltinVoid = Builtin(ElementBuilder("builtin").addAttr("name", "Void").addAttr("validation", '{}'))
BuiltinInteger = Builtin(ElementBuilder("builtin").addAttr("name", "Integer").addAttr("validation", '{"0": "integer"}'))
BuiltinFloat = Builtin(ElementBuilder("builtin").addAttr("name", "Float").addAttr("validation", '{"0": "float"}'))
BuiltinBoolean = Builtin(ElementBuilder("builtin").addAttr("name", "Boolean").addAttr("validation", '{"0": "boolean"}'))
BuiltinResult = Builtin(
	ElementBuilder("builtin").addAttr("name", "Result").addAttr("validation_template",
	'{"0": "mandatory", "1": "mandatory"}'))
BuiltinList = Builtin(
	ElementBuilder("builtin").addAttr("name", "List").addAttr("validation_template", '{"0": "mandatory"}'))
BuiltinCallable = Builtin(ElementBuilder("builtin").addAttr("name", "Callable").addAttr("validation", '{}'))

Builtins = [BuiltinVoid, BuiltinInteger, BuiltinFloat, BuiltinBoolean, BuiltinResult, BuiltinList, BuiltinCallable]
