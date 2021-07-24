import typing

from tools.bdl.contracts.traits import ContractTraits, Role


class ContractString(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="string", role=Role.Value | Role.Public, validationSchema=[])
