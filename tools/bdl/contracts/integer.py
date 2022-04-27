import typing

from bzd.validation.constraints.integer import Integer
from bzd.validation.schema import TypeContext

from tools.bdl.contracts.traits import ContractTraits, Role


class Integer_(Integer):

	def check(self, context: TypeContext) -> None:

		assert "resolver" in context.args

		from tools.bdl.entities.impl.entity import Entity
		if isinstance(context.value, Entity):
			if context.value.underlyingType == "Integer":
				value = context.args["resolver"].getEntityResolved(context.value.underlyingValue).value
				context.value = value.parametersResolved[0].value

		super().check(context)


class ContractInteger(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="integer", role=Role.Value | Role.Template | Role.Public, constraint=Integer_)
