from bdl.contracts.contract import Contract
from bdl.contracts.traits import ContractTraits, Role


class ContractMax(ContractTraits):
	def __init__(self) -> None:
		super().__init__(name="max", role=Role.Value | Role.Template | Role.Public)

	def resolveConflictInternals(self, base: Contract, derived: Contract) -> Contract | None:
		if base.valueNumber > derived.valueNumber:
			raise RuntimeError(f"base contract containing [{base!s}] is defined with a lower maximum.")
		return derived
