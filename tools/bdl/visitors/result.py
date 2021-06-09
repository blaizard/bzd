import typing
from pathlib import Path

from tools.bdl.result import ResultType

from tools.bdl.visitor import Visitor as VisitorBase
from tools.bdl.entity.variable import Variable
from tools.bdl.entity.nested import Nested
from tools.bdl.entity.method import Method
from tools.bdl.entity.using import Using
from tools.bdl.entity.enum import Enum
from tools.bdl.entity.namespace import Namespace
from tools.bdl.entity.use import Use

T = typing.TypeVar("T")

class Visitor(VisitorBase[ResultType, T]):

	def visitBegin(self, result: typing.Any) -> ResultType:
		return ResultType(level=self.level)

	def visitNestedEntities(self, entity: Nested, result: ResultType) -> None:
		result.registerSymbol(entity=entity)

	def visitVariable(self, entity: Variable, result: ResultType) -> None:
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
