import typing

from bzd.validation.schema import Context, Constraint, ProcessedSchema

from tools.bdl.contracts.traits import ContractTraits, Role


class Convertible_(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:

		def _process(base: str, context: Context) -> None:
			assert "resolver" in context.args

			if base == "Any":
				return True

			assert context.value.isType, "The expression is not a type."
			typeFrom = context.value.type

			# It must be found.
			entityTo = context.args["resolver"].getEntityResolved(base).value
			assert entityTo.isType, "The base FQN does not resolve to a type."
			assert typeFrom.isConvertible(resolver=context.args["resolver"],
				to=entityTo.type), f"Type '{typeFrom}' is not convertible to type '{entityTo.type}'."

		updatedArgs = self.validate(schema=["mandatory string"], values=args).values
		processedSchema.installValidation(_process, updatedArgs[0])


class ContractConvertible(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="convertible", role=Role.Template, constraint=Convertible_)
