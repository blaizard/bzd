import typing

from tools.bdl.contracts.traits import ContractTraits


class ContractFloat(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="float", isValue=True, validationSchema=[])
