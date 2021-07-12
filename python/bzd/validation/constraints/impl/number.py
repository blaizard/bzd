import typing

from bzd.validation.schema import Args, Result, Context, Constraint, ProcessedSchema

T = typing.TypeVar("T")


class Number(typing.Generic[T], Constraint):

	def install(self, processedSchema: ProcessedSchema, args: Args) -> None:
		processedSchema.setType(self)

	def min(self, processedSchema: ProcessedSchema, args: Args) -> None:

		def _process(arg: float, context: Context) -> Result:
			if context.underlying < arg:
				return "The value {} is lower than the required minimum: {}".format(context.underlying, arg)
			return None

		assert len(args) == 1, "Requires a single argument."
		processedSchema.installValidation(_process, float(args[0]))

	def max(self, processedSchema: ProcessedSchema, args: Args) -> None:

		def _process(arg: float, context: Context) -> Result:
			if context.underlying > arg:
				return "The value {} is higher than the required maximum: {}".format(context.underlying, arg)
			return None

		assert len(args) == 1, "Requires a single argument."
		processedSchema.installValidation(_process, float(args[0]))
