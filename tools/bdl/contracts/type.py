import typing

from tools.bdl.contracts.traits import ContractTraits, Role


class ContractType(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="type", role=Role.Template, validationSchema=[])
