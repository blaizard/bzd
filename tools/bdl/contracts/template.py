import typing

from tools.bdl.contracts.traits import ContractTraits


class ContractTemplate(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="template", isValue=False, validationSchema=[])
