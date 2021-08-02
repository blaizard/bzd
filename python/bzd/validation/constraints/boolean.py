import typing

from bzd.validation.schema import Args, Result, TypeContext, Constraint, ProcessedSchema
import bzd.validation.validation


class Boolean(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: Args) -> None:
		bzd.validation.validation.Validation(schema=[]).validate(args)
		processedSchema.setType(self)

	def check(self, context: TypeContext) -> Result:
		if context.value in {"true", "True"}:
			context.setUnderlying(True)
		elif context.value in {"false", "False"}:
			context.setUnderlying(False)
		else:
			return "expects a boolean, received: '{}'.".format(context.value)
		return None
