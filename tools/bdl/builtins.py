from bzd.parser.element import Element
from tools.bdl.visitor import Visitor
from tools.bdl.entities.impl.builtin import Builtin

BuiltinVoid = Builtin(Element.fromSerialize(Visitor.makeEntity("builtin", values={"name": "Void", "validation": '{}'})))
BuiltinInteger = Builtin(
	Element.fromSerialize(Visitor.makeEntity("builtin", values={
	"name": "Integer",
	"validation": '{"0": "integer"}'
	})))
BuiltinFloat = Builtin(
	Element.fromSerialize(Visitor.makeEntity("builtin", values={
	"name": "Float",
	"validation": '{"0": "float"}'
	})))
BuiltinBoolean = Builtin(
	Element.fromSerialize(Visitor.makeEntity("builtin", values={
	"name": "Boolean",
	"validation": '{"0": "boolean"}'
	})))
BuiltinResult = Builtin(
	Element.fromSerialize(
	Visitor.makeEntity("builtin",
	values={
	"name": "Result",
	"validation_template": '{"0": "mandatory", "1": "mandatory"}'
	})))
BuiltinList = Builtin(
	Element.fromSerialize(
	Visitor.makeEntity("builtin", values={
	"name": "List",
	"validation_template": '{"0": "mandatory"}'
	})))
BuiltinCallable = Builtin(
	Element.fromSerialize(Visitor.makeEntity("builtin", values={
	"name": "Callable",
	"validation": '{}'
	})))

Builtins = [BuiltinVoid, BuiltinInteger, BuiltinFloat, BuiltinBoolean, BuiltinResult, BuiltinList, BuiltinCallable]
