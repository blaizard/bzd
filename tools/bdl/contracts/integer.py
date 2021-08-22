import typing

from bzd.validation.constraints.integer import Integer as IntegerBase
from bzd.validation.schema import TypeContext, Result

from tools.bdl.contracts.traits import ContractTraits, Role


class Integer(IntegerBase):

	def check(self, context: TypeContext) -> Result:
		return super().check(context)


class ContractInteger(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="integer", role=Role.Value | Role.Template | Role.Public, constraint=Integer)
