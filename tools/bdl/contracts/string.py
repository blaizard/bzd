import typing

from tools.bdl.contracts.traits import ContractTraits


class ContractString(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="string", isValue=True, validationSchema=[])
