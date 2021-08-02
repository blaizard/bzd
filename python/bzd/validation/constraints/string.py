import typing

from bzd.validation.schema import Args, Result, TypeContext, Context, Constraint, ProcessedSchema
import bzd.validation.validation


class String(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: Args) -> None:
		bzd.validation.validation.Validation(schema=[]).validate(args)
		processedSchema.setType(self)

	def check(self, context: TypeContext) -> Result:
		return None

	def min(self, processedSchema: ProcessedSchema, args: Args) -> None:

		def _process(arg: int, context: Context) -> Result:
			if len(context.value) < arg:
				return "the string '{}' is shorter than its required minimum size: {}".format(context.value, arg)
			return None

		updatedArgs = bzd.validation.validation.Validation(schema=["mandatory integer"]).validate(args).valuesAsList
		processedSchema.installValidation(_process, updatedArgs[0])

	def max(self, processedSchema: ProcessedSchema, args: Args) -> None:

		def _process(arg: int, context: Context) -> Result:
			if len(context.value) > arg:
				return "the string '{}' is longer than its required maximum size: {}".format(context.value, arg)
			return None

		updatedArgs = bzd.validation.validation.Validation(schema=["mandatory integer"]).validate(args).valuesAsList
		processedSchema.installValidation(_process, updatedArgs[0])
