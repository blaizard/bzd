import typing

from tools.bdl.contracts.traits import ContractTraits


class ContractMandatory(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="mandatory", isValue=True, validationSchema=[])
