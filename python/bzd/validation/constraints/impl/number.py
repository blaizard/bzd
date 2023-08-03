import typing

from bzd.validation.schema import Context, Constraint, ProcessedSchema

T = typing.TypeVar("T")


class Min(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:

		def _process(arg: float, context: Context) -> None:
			assert (context.underlying
			        >= arg), f"the value '{context.underlying}' is lower than the required minimum: {arg}"

		updatedArgs = self.validate(schema=["mandatory float"], values=args).values
		processedSchema.installValidation(_process, updatedArgs[0])


class Max(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:

		def _process(arg: float, context: Context) -> None:
			assert (context.underlying
			        <= arg), f"the value '{context.underlying}' is higher than the required maximum: {arg}"

		updatedArgs = self.validate(schema=["mandatory float"], values=args).values
		processedSchema.installValidation(_process, updatedArgs[0])


class Number(typing.Generic[T], Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:
		self.validate(schema=[], values=args)
		processedSchema.setType(self)

	constraints = {"min": Min, "max": Max}
