import typing

from bzd.validation.schema import Args, Result, Context, Constraint, ProcessedSchema


class Integer(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: Args) -> None:
		processedSchema.setType(self)

	def toType(self, context: Context) -> typing.Optional[int]:
		"""
		Check that the value match the type
		and return the underlying type.
		"""
		try:
			converted = float(context.value)
			assert converted.is_integer()
			return int(converted)
		except:
			return None

	def min(self, processedSchema: ProcessedSchema, args: Args) -> None:
		def _process(arg: float, context: Context) -> Result:
			if context.underlying < arg:
				return "Value lower than required minimum: {}".format(arg)
			return None

		assert len(args) == 1, "Requires a single argument."
		processedSchema.installValidation(_process, float(args[0]))

	def max(self, processedSchema: ProcessedSchema, args: Args) -> None:
		def _process(arg: float, context: Context) -> Result:
			if context.underlying > arg:
				return "Value higher than required maximum: {}".format(arg)
			return None

		assert len(args) == 1, "Requires a single argument."
		processedSchema.installValidation(_process, float(args[0]))
