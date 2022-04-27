import typing

import bzd.validation.validation
from bzd.validation.schema import Constraint, ProcessedSchema

from tools.bdl.contracts.traits import ContractTraits, Role


class CapacityConstraint(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:
		bzd.validation.validation.Validation(schema=["integer min(1) mandatory"]).validate(args)


class ContractCapacity(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="capacity", role=Role.Meta | Role.Public, constraint=CapacityConstraint)
