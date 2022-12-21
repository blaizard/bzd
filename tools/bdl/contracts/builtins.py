import typing

from bzd.validation.constraints.integer import Integer
from bzd.validation.constraints.float import Float
from bzd.validation.constraints.boolean import Boolean
from bzd.validation.schema import TypeContext

from tools.bdl.contracts.traits import ContractTraits, Role

def getValueFromSingleParameter_(resolver, typeFQN: str, value: typing.Any, defaultValue: str) -> typing.Any:
	"""Extract value of single parameter expressions."""

	from tools.bdl.entities.impl.expression import Expression
	if isinstance(value, Expression):
		if value.isLiteral:
			return value.literal
		elif value.underlyingTypeFQN == typeFQN:
			# Look if there is an underlying value.
			if value.underlyingValueFQN is not None:
				value = resolver.getEntityResolved(value.underlyingValueFQN).value
				assert isinstance(value, Expression), "An underlying value FQN must point to an expression."
			# Get the value from the parameter, at this point we have an expression like `typeFQN(...)`.
			if value.parametersResolved.size() == 0:
				return defaultValue
			elif value.parametersResolved.size() == 1:
				return value.parametersResolved[0].param.literal
			raise Exception(f"There are too many initializers for '{typeFQN}', only 1 is expected:\n{value.parametersResolved}")

	return value

class Integer_(Integer):

	def check(self, context: TypeContext) -> None:

		assert "resolver" in context.args
		context.value = getValueFromSingleParameter_(context.args["resolver"], "Integer", context.value, "0")
		super().check(context)

class Float_(Float):

	def check(self, context: TypeContext) -> None:

		assert "resolver" in context.args
		context.value = getValueFromSingleParameter_(context.args["resolver"], "Float", context.value, "0")
		super().check(context)

class Boolean_(Boolean):

	def check(self, context: TypeContext) -> None:

		assert "resolver" in context.args
		context.value = getValueFromSingleParameter_(context.args["resolver"], "Boolean", context.value, "false")
		super().check(context)

class ContractInteger(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="integer", role=Role.Value | Role.Public, constraint=Integer_)

class ContractFloat(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="float", role=Role.Value | Role.Public, constraint=Float_)

class ContractBoolean(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="boolean", role=Role.Value | Role.Public, constraint=Boolean_)
