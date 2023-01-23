import typing

from bzd.validation.schema import Context, Constraint, ProcessedSchema

from tools.bdl.contracts.traits import ContractTraits, Role


class Convertible_(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:

		def _process(base: str, context: Context) -> None:
			assert "resolver" in context.args

			if base == "Any":
				return

			from tools.bdl.entities.impl.entity import EntityExpression
			assert isinstance(context.value, EntityExpression), "The value must be an expression."
			assert context.value.isSymbol, "The expression is not a type."
			typeFrom = context.value.symbol

			# It must be found.
			entityTo = context.args["resolver"].getEntityResolved(base).value
			assert entityTo.isSymbol, "The base FQN does not resolve to a type."
			assert typeFrom.isConvertible(resolver=context.args["resolver"],
				to=entityTo.symbol), f"Type '{typeFrom}' is not convertible to type '{entityTo.symbol}'."

		updatedArgs = self.validate(schema=["mandatory string"], values=args).values
		processedSchema.installValidation(_process, updatedArgs[0])


class ContractConvertible(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="convertible", role=Role.Template, constraint=Convertible_)
