import typing
from pathlib import Path

from bzd.parser.element import Element
from bzd.parser.error import Error

from tools.bdl.result import ResultType, SymbolType
from tools.bdl.visitors.map import MapType
from tools.bdl.visitor import Visitor as VisitorBase

from tools.bdl.entity.variable import Variable
from tools.bdl.entity.builtin import Builtin
from tools.bdl.entity.nested import Nested
from tools.bdl.entity.method import Method
from tools.bdl.entity.using import Using
from tools.bdl.entity.enum import Enum
from tools.bdl.entity.namespace import Namespace
from tools.bdl.entity.use import Use
from tools.bdl.entity.fragment.type import Type

ElementsMap = typing.Dict[str, Element]


class Result(VisitorBase[ResultType]):

	def __init__(self, symbols: MapType) -> None:
		super().__init__()
		self.symbols = {
			"Integer": self.makeBuiltin("Integer"),
			"Float": self.makeBuiltin("Float"),
			"Result": self.makeBuiltin("Result"),
			"Callable": self.makeBuiltin("Callable")
		}
		self.elements: ElementsMap = {}

		self.registerSymbols(symbols)

	def makeBuiltin(self, name: str) -> typing.Any:
		return {"@": {"name": {"v": name, "i": 0}, "category": {"v": "builtin", "i": 0}}}

	def registerSymbols(self, symbols: MapType) -> None:
		"""
		Add new symbols to the current symbol list.
		"""
		for key, element in symbols.items():
			assert key not in self.elements, "Symbol conflict '{}'.".format(key)
			self.symbols[key] = element

	def visitBegin(self, result: typing.Any) -> ResultType:
		return ResultType(level=self.level)

	def resolveTypeIfAny(self, entity: typing.Optional[Type]) -> None:
		"""
		Resolve a symbol looking at the symbol map going up into the namespaces.
		"""
		if entity is None:
			return
		name = entity.kind

		# Look for a symbol match
		namespace = self.namespace.copy()
		while True:
			symbol = self.makeFQN(name=name, namespace=namespace)
			if symbol in self.symbols:
				break
			# Failed to match any symbol from the map
			Error.assertTrue(element=entity.element,
				condition=bool(namespace),
				message="Symbol '{}' in namespace '{}' could not be resolved.".format(name, ".".join(self.namespace)))
			namespace.pop()

		# Save the entity
		if symbol not in self.elements:
			element = Element.fromSerialize(element=self.symbols[symbol])
			self.elements[symbol] = element
		entity.setUnderlying(self.elementToEntity(element))

	def visitNestedEntities(self, entity: Nested, result: ResultType) -> None:
		result.registerSymbol(entity=entity)

	def visitVariable(self, entity: Variable, result: ResultType) -> None:
		result.registerSymbol(entity=entity)

	def visitMethod(self, entity: Method, result: ResultType) -> None:
		result.registerSymbol(entity=entity)
		self.resolveTypeIfAny(entity=entity.type)
		for arg in entity.args:
			self.resolveTypeIfAny(entity=arg.type)

	def visitUsing(self, entity: Using, result: ResultType) -> None:
		result.registerSymbol(entity=entity)

	def visitEnum(self, entity: Enum, result: ResultType) -> None:
		result.registerSymbol(entity=entity)

	def visitNamespace(self, entity: Namespace, result: ResultType) -> None:
		result.registerNamespace(entity=entity)

	def visitUse(self, entity: Use, result: ResultType) -> None:
		result.registerUse(entity=entity)
