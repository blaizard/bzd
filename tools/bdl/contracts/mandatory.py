import typing

from tools.bdl.contracts.traits import ContractTraits, Role


class ContractMandatory(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="mandatory", role=Role.Value | Role.Template, validationSchema=[])
