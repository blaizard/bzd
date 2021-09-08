import typing

from bzd.validation.schema import NoopConstraint

from tools.bdl.contracts.traits import ContractTraits, Role


class ContractVirtual(ContractTraits):

	def __init__(self) -> None:
		super().__init__(name="virtual", role=Role.Meta | Role.Public, constraint=NoopConstraint)
