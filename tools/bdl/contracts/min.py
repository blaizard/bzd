import typing

from tools.bdl.contracts.traits import ContractTraits
from tools.bdl.contracts.contract import Contract


class ContractMin(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="min", isValue=True, validationSchema=["integer mandatory"])

	def resolveConflictInternals(self, base: Contract, derived: Contract) -> typing.Optional[Contract]:
		if base.valueNumber > derived.valueNumber:
			raise Exception("base contract containing [{}] is defined with a higher minimum.".format(str(base)))
		return derived
