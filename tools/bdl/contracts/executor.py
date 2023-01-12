import typing

import bzd.validation.validation
from bzd.validation.schema import Constraint, ProcessedSchema, TypeContext
from tools.bdl.entities.impl.types import Category

from tools.bdl.contracts.traits import ContractTraits, Role


class ExecutorConstraint_(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:
		self.validate(schema=["string"], values=args)


class ContractExecutor(ContractTraits):
	"""Bind an expression to an executor."""

	def __init__(self) -> None:
		super().__init__(name="executor", role=Role.Entity | Role.Public, constraint=ExecutorConstraint_)
