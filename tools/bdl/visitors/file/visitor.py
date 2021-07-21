import typing
from pathlib import Path

from bzd.parser.element import Element, ElementSerialize
from bzd.parser.error import Error

from tools.bdl.visitor import Visitor as VisitorBase
from tools.bdl.object import Object
from tools.bdl.visitors.file.nested_result import NestedResult
from tools.bdl.visitors.file.result import ResultType
from tools.bdl.entities.all import Expression, Builtin, Nested, Method, Using, Enum, Namespace, Use, SymbolType
from tools.bdl.entities.impl.fragment.type import Type


class Visitor(VisitorBase[ResultType]):

	def __init__(self, bdl: Object, resolve: bool = False, include: bool = False) -> None:
		"""
		Create a result object.
		- Params:
			resolve: Resolve symbols.
			include: Follow use statement.
		"""

		super().__init__(elementToEntityExtenstion={"nested": NestedResult})
		self.bdl = bdl

		# Options
		self.isResolve = resolve
		self.isInclude = include

	def process(self) -> ResultType:
		return self.visit(self.bdl.parsed)

	def visitBegin(self, result: typing.Any) -> ResultType:
		return ResultType(level=self.level)

	def entityToNested(self, category: str, entity: Nested, nested: ResultType) -> None:
		nestedResult = typing.cast(NestedResult, entity)
		nestedResult.setNested(category=category, nested=typing.cast(typing.List[typing.Any], nested))

	def visitNestedEntities(self, entity: Nested, result: ResultType) -> None:
		result.registerSymbol(entity=entity)

	def visitExpression(self, entity: Expression, result: ResultType) -> None:
		result.registerSymbol(entity=entity)

	def visitMethod(self, entity: Method, result: ResultType) -> None:
		result.registerSymbol(entity=entity)

	def visitUsing(self, entity: Using, result: ResultType) -> None:
		result.registerSymbol(entity=entity)

	def visitEnum(self, entity: Enum, result: ResultType) -> None:
		result.registerSymbol(entity=entity)

	def visitNamespace(self, entity: Namespace, result: ResultType) -> None:
		result.registerNamespace(entity=entity)

	def visitUse(self, entity: Use, result: ResultType) -> None:
		result.registerUse(entity=entity)
