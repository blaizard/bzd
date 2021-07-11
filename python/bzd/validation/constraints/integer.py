import typing

from bzd.validation.schema import Args, Result, TypeContext, Context, Constraint, ProcessedSchema


class Integer(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: Args) -> None:
		processedSchema.setType(self)

	def check(self, context: TypeContext) -> Result:
		"""
		Check that the value match the type and save the underlying type.
		"""
		try:
			context.setUnderlying(self._toInteger(context.value))
		except Exception as e:
			return str(e)
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
