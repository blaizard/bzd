import typing

from bzd.validation.constraints.mandatory import Mandatory

from bdl.contracts.traits import ContractTraits, Role


class ContractMandatory(ContractTraits):

	def __init__(self) -> None:
		super().__init__(
		    name="mandatory",
		    role=Role.Value | Role.Template | Role.Public,
		    constraint=Mandatory,
		)
