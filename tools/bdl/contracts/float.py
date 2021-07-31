import typing

from tools.bdl.contracts.traits import ContractTraits, Role


class ContractFloat(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="float", role=Role.Value | Role.Template | Role.Public, validationSchema=["float"])
