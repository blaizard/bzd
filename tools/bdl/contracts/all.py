import typing

from bzd.utils.classproperty import classproperty

from tools.bdl.contracts.contract import Contract
from tools.bdl.contracts.min import ContractMin
from tools.bdl.contracts.max import ContractMax
from tools.bdl.contracts.template import ContractTemplate

_Contracts = [ContractMin(), ContractMax(), ContractTemplate()]


class AllContracts:
	"""Contains all contracts."""

	@classproperty
	def all() -> typing.Mapping[str, Contract]:  # type: ignore
		return {contract.name: contract for contract in _Contracts}  # type: ignore

	@classproperty
	def forValues() -> typing.Mapping[str, Contract]:  # type: ignore
		return {contract.name: contract for contract in _Contracts if contract.isValue}  # type: ignore
