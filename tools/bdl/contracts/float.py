import typing

from bzd.validation.constraints.float import Float as FloatBase
from bzd.validation.schema import TypeContext, Result

from tools.bdl.contracts.traits import ContractTraits, Role


class Float(FloatBase):

	def check(self, context: TypeContext) -> Result:

		assert "symbols" in context.args
		from tools.bdl.entities.impl.entity import Entity

		if isinstance(context.value, Entity):
			if context.value.underlyingType == "Float":
				value = context.args["symbols"].getEntityResolved(context.value.underlyingValue).value
				assert len(value.parametersResolved) == 1
				context.value = value.parametersResolved[0].value

		return super().check(context)


class ContractFloat(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="float", role=Role.Value | Role.Template | Role.Public, constraint=Float)
