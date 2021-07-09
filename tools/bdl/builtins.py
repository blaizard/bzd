from bzd.parser.element import Element
from tools.bdl.visitor import Visitor
from tools.bdl.entities.impl.builtin import Builtin

BuiltinVoid = Builtin(
	Element.fromSerialize(Visitor.makeEntity("builtin", values={
	"name": "Void",
	"validation": "none"
	})))
BuiltinInteger = Builtin(
	Element.fromSerialize(Visitor.makeEntity("builtin", values={
	"name": "Integer",
	"validation": "integer"
	})))
BuiltinFloat = Builtin(
	Element.fromSerialize(Visitor.makeEntity("builtin", values={
	"name": "Float",
	"validation": "float"
	})))
BuiltinResult = Builtin(
	Element.fromSerialize(
	Visitor.makeEntity("builtin",
	values={"name": "Result"},
	sequences={"templates": [{
	"validation": "mandatory"
	}, {
	"validation": "mandatory"
	}]})))
BuiltinList = Builtin(
	Element.fromSerialize(
	Visitor.makeEntity("builtin",
	values={"name": "List"},
	sequences={"templates": [{
	"validation": "mandatory integer"
	}]})))
BuiltinCallable = Builtin(Element.fromSerialize(Visitor.makeEntity("builtin", values={"name": "Callable"})))

Builtins = [BuiltinVoid, BuiltinInteger, BuiltinFloat, BuiltinResult, BuiltinList, BuiltinCallable]
