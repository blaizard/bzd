import typing

from bzd.validation.schema import Result, Context, Constraint, ValidationCallable, ProcessedSchema


class Integer(Constraint):

	def install(self, processedSchema: "ProcessedSchema") -> None:

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

	def min(self, context: Context) -> Result:
		return None

	def max(self, context: Context) -> Result:
		return None
