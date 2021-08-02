import typing

from tools.bdl.contracts.traits import ContractTraits, Role


class ContractTemplate(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="template", role=Role.Meta | Role.Public)
