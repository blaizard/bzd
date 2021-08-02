import typing

from bzd.validation.schema import Args, Result, Context, Constraint, ProcessedSchema
import bzd.validation.validation

T = typing.TypeVar("T")


class Number(typing.Generic[T], Constraint):

	def install(self, processedSchema: ProcessedSchema, args: Args) -> None:
		bzd.validation.validation.Validation(schema=[]).validate(args)
		processedSchema.setType(self)

	def min(self, processedSchema: ProcessedSchema, args: Args) -> None:

		def _process(arg: float, context: Context) -> Result:
			if context.underlying < arg:
				return "the value {} is lower than the required minimum: {}".format(context.underlying, arg)
			return None

		updatedArgs = bzd.validation.validation.Validation(schema=["mandatory float"]).validate(args).valuesAsList
		processedSchema.installValidation(_process, updatedArgs[0])

	def max(self, processedSchema: ProcessedSchema, args: Args) -> None:

		def _process(arg: float, context: Context) -> Result:
			if context.underlying > arg:
				return "the value {} is higher than the required maximum: {}".format(context.underlying, arg)
			return None

		updatedArgs = bzd.validation.validation.Validation(schema=["mandatory float"]).validate(args).valuesAsList
		processedSchema.installValidation(_process, updatedArgs[0])
