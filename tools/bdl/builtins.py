from bzd.parser.element import Element
from tools.bdl.visitor import Visitor
from tools.bdl.entities.impl.builtin import Builtin

BuiltinInteger = Builtin(Element.fromSerialize(Visitor.makeEntity("builtin", values={"name": "Integer"})))
BuiltinFloat = Builtin(Element.fromSerialize(Visitor.makeEntity("builtin", values={"name": "Float"})))
BuiltinResult = Builtin(Element.fromSerialize(Visitor.makeEntity("builtin", values={"name": "Result"})))
BuiltinList = Builtin(Element.fromSerialize(Visitor.makeEntity("builtin", values={"name": "List"})))
BuiltinCallable = Builtin(Element.fromSerialize(Visitor.makeEntity("builtin", values={"name": "Callable"})))

Builtins = [BuiltinInteger, BuiltinFloat, BuiltinResult, BuiltinList, BuiltinCallable]
