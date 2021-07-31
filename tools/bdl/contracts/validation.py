import typing

from bzd.validation.validation import Validation as ValidationBase
from bzd.validation.schema import Args, Constraint, ProcessedSchema, TypeContext, Result


# TODO: to incorporate within type.py
class TypeConstraint(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: Args) -> None:
		processedSchema.setType(self)

	def check(self, context: TypeContext) -> Result:
		if "fragment.type.Type" in str(type(context.value)):
			return None
		return "'{}' is not a type.".format(context.value)


class Validation(ValidationBase):

	AVAILABLE_CONSTRAINTS: typing.Dict[str, typing.Type[Constraint]] = {
		"type": TypeConstraint,
		**ValidationBase.AVAILABLE_CONSTRAINTS
	}
