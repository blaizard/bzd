import typing

from bzd.validation.schema import TypeContext, Constraint, ProcessedSchema


class Boolean(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:
		self.validate(schema=[], values=args)
		processedSchema.setType(self)

	def check(self, context: TypeContext) -> None:
		if context.value in {"true", "True"}:
			context.setUnderlying(True)
		elif context.value in {"false", "False"}:
			context.setUnderlying(False)
		else:
			raise Exception(f"expects a boolean, received: '{context.value}'.")
