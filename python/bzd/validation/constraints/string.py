import typing

from bzd.validation.schema import TypeContext, Context, Constraint, ProcessedSchema


class Min(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:

		def _process(arg: int, context: Context) -> None:
			assert len(
			    context.value) >= arg, f"the string '{context.value}' is shorter than its required minimum size: {arg}"

		updatedArgs = self.validate(schema=["mandatory integer"], values=args).values
		processedSchema.installValidation(_process, updatedArgs[0])


class Max(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:

		def _process(arg: int, context: Context) -> None:
			assert len(
			    context.value) <= arg, f"the string '{context.value}' is longer than its required maximum size: {arg}"

		updatedArgs = self.validate(schema=["mandatory integer"], values=args).values
		processedSchema.installValidation(_process, updatedArgs[0])


class String(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:
		self.validate(schema=[], values=args)
		processedSchema.setType(self)

	constraints = {"min": Min, "max": Max}
