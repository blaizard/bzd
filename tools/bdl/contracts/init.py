import typing

import bzd.validation.validation
from bzd.validation.schema import Constraint, ProcessedSchema, TypeContext

from tools.bdl.contracts.traits import ContractTraits, Role


class InitConstraint_(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:
		self.validate(schema=["string"] * len(args), values=args)

	def check(self, context: TypeContext) -> None:

		assert "resolver" in context.args

		from tools.bdl.entities.impl.entity import Entity
		if isinstance(context.value, Entity):
			underlyingType = context.args["resolver"].getEntityResolved(context.value.underlyingType).value
			assert underlyingType.category == "method", "'init' constraint applies only to methods."


class ContractInit(ContractTraits):
	"""Tag a function as an initializer."""

	def __init__(self) -> None:
		super().__init__(name="init", role=Role.Entity | Role.Public, constraint=InitConstraint_)
