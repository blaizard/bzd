import typing

import bzd.validation.validation
from bzd.validation.schema import Constraint, ProcessedSchema, TypeContext
from bdl.entities.impl.types import Category

from bdl.contracts.traits import ContractTraits, Role


class ShutdownConstraint_(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:
		self.validate(schema=["string"] * len(args), values=args)

	def check(self, context: TypeContext) -> None:
		assert "resolver" in context.args

		from bdl.entities.impl.entity import Entity

		if isinstance(context.value, Entity):
			underlyingTypeFQN = (context.args["resolver"].getEntityResolved(context.value.underlyingTypeFQN).value)
			assert (underlyingTypeFQN.category == Category.method), "'shutdown' constraint applies only to methods."
			assert (underlyingTypeFQN.parameters.empty()), "'shutdown' methods cannot take arguments."


class ContractShutdown(ContractTraits):
	"""Tag a function as an terminator."""

	def __init__(self) -> None:
		super().__init__(
		    name="shutdown",
		    role=Role.Entity | Role.Public,
		    constraint=ShutdownConstraint_,
		)
