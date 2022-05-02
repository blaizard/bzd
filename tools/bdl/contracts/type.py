import typing

from bzd.validation.schema import Constraint, Constraint, ProcessedSchema, TypeContext

from tools.bdl.contracts.traits import ContractTraits, Role


class TypeConstraint_(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:
		processedSchema.setType(self)

	def check(self, context: TypeContext) -> None:
		from tools.bdl.entities.impl.fragment.type import Type
		assert isinstance(context.value, Type), f"'{context.value}' is not a type."


class ContractType(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="type", role=Role.Template | Role.Public, constraint=TypeConstraint_)
