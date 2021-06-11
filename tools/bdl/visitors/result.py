import typing
from pathlib import Path

from bzd.parser.element import Element, ElementSerialize
from bzd.parser.error import Error

from tools.bdl.result import ResultType, SymbolType
from tools.bdl.visitors.map import MapType
from tools.bdl.visitor import Visitor as VisitorBase
from tools.bdl.object import Object

from tools.bdl.entities.all import Variable, Builtin, Nested, Method, Using, Enum, Namespace, Use
from tools.bdl.entities.impl.fragment.type import Type


class Result(VisitorBase[ResultType]):

	def __init__(self, bdl: Object) -> None:
		super().__init__()
		self.bdl = bdl

		self.bdl.registerSymbols({
			"Integer": self.makeEntity("builtin", values={"name": "Integer"}),
			"Float": self.makeEntity("builtin", values={"name": "Float"}),
			"Result": self.makeEntity("builtin", values={"name": "Result"}),
			"Callable": self.makeEntity("builtin", values={"name": "Callable"})
		})

	def process(self) -> ResultType:
		return self.visit(self.bdl.parsed)

	def visitBegin(self, result: typing.Any) -> ResultType:
		return ResultType(level=self.level)

	def resolveTypeIfAny(self, entity: typing.Optional[Type]) -> None:
		"""
		Resolve a symbol looking at the symbol map going up into the namespaces.
		"""
		if entity is None:
			return

		# Look for a match
		maybeElement = self.bdl.getElementFromType(entity=entity, namespace=self.namespace)
		if not maybeElement:
			# Failed to match any symbol from the map
			Error.handleFromElement(element=entity.element,
				message="Symbol '{}' in namespace '{}' could not be resolved.".format(entity.kind, ".".join(self.namespace)))

		# Save the underlying element
		assert maybeElement
		entity.setUnderlying(self.elementToEntity(maybeElement))

	def visitNestedEntities(self, entity: Nested, result: ResultType) -> None:
		result.registerSymbol(entity=entity)
		for inheritance in entity.inheritanceList:
			self.resolveTypeIfAny(entity=inheritance)

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
		bdl = Object.fromPath(entity.path)
		self.bdl.registerSymbols(bdl.symbols)
