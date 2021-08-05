import typing

from bzd.validation.schema import Args, Constraint, ProcessedSchema, TypeContext, Result

from tools.bdl.contracts.traits import ContractTraits, Role


class TypeConstraint(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: Args) -> None:
		processedSchema.setType(self)

	def check(self, context: TypeContext) -> Result:
		from tools.bdl.entities.impl.fragment.type import Type
		if isinstance(context.value, Type):
			return None
		return "'{}' is not a type.".format(context.value)


class ContractType(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="type", role=Role.Template | Role.Public, constraint=TypeConstraint)
