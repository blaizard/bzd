import typing

from bdl.contracts.traits import ContractTraits, Role
from bdl.contracts.contract import Contract


class ContractMax(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="max", role=Role.Value | Role.Template | Role.Public)

	def resolveConflictInternals(self, base: Contract, derived: Contract) -> typing.Optional[Contract]:
		if base.valueNumber > derived.valueNumber:
			raise Exception("base contract containing [{}] is defined with a lower maximum.".format(str(base)))
		return derived
