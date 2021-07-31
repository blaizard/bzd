import typing

from tools.bdl.contracts.traits import ContractTraits, Role


class ContractBoolean(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="boolean", role=Role.Value | Role.Public, validationSchema=["boolean"])
