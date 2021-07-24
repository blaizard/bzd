import typing

from tools.bdl.contracts.traits import ContractTraits, Role


class ContractInteger(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="integer", role=Role.Value | Role.Template | Role.Public, validationSchema=[])
