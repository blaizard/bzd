import typing

from bzd.validation.constraints.float import Float
from bzd.validation.schema import TypeContext

from tools.bdl.contracts.traits import ContractTraits, Role


class Float_(Float):

	def check(self, context: TypeContext) -> None:

		assert "resolver" in context.args

		from tools.bdl.entities.impl.entity import Entity
		if isinstance(context.value, Entity):
			if context.value.underlyingTypeFQN == "Float":
				# Look if there is an underlying value.
				if context.value.underlyingValueFQN is not None:
					context.value = context.args["resolver"].getEntityResolved(context.value.underlyingValueFQN).value
				# Ensure that there is only one argument to the function and read it.
				assert context.value.parametersResolved.size() == 1
				context.value = context.value.parametersResolved[0].value

		super().check(context)


class ContractFloat(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="float", role=Role.Value | Role.Public, constraint=Float_)
