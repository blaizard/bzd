import typing

from tools.bdl.contracts.traits import ContractTraits


class ContractBoolean(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="boolean", isValue=True, validationSchema=[])
