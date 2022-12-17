import typing

from bzd.validation.constraints.integer import Integer
from bzd.validation.schema import TypeContext

from tools.bdl.contracts.traits import ContractTraits, Role


class Integer_(Integer):

	def check(self, context: TypeContext) -> None:

		assert "resolver" in context.args

		from tools.bdl.entities.impl.expression import Expression
		if isinstance(context.value, Expression):
			if context.value.underlyingTypeFQN == "Integer":
				# Look if there is an underlying value.
				if context.value.underlyingValueFQN is not None:
					context.value = context.args["resolver"].getEntityResolved(context.value.underlyingValueFQN).value
				# Ensure that there is only one argument to the function and read it.
				assert context.value.parametersResolved.size() == 1
				context.value = context.value.parametersResolved[0].value

		super().check(context)


class ContractInteger(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="integer", role=Role.Value | Role.Template | Role.Public, constraint=Integer_)
