from bzd.validation.schema import Constraint, ProcessedSchema, TypeContext


class Boolean(Constraint):
	def install(self, processedSchema: ProcessedSchema, args: list[str]) -> None:
		self.validate(schema=[], values=args)
		processedSchema.setType(self)

	def check(self, context: TypeContext) -> None:
		if context.value in {"true", "True"}:
			context.setUnderlying(True)
		elif context.value in {"false", "False"}:
			context.setUnderlying(False)
		else:
			raise RuntimeError(f"expects a boolean, received: '{context.value}'.")
