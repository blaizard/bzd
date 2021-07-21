import typing

from tools.bdl.contracts.traits import ContractTraits


class ContractInteger(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="integer", isValue=True, validationSchema=[])
