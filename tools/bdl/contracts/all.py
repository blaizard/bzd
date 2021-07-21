import typing

from bzd.utils.decorators import cached_classproperty

from tools.bdl.contracts.traits import ContractTraits
from tools.bdl.contracts.min import ContractMin
from tools.bdl.contracts.max import ContractMax
from tools.bdl.contracts.template import ContractTemplate
from tools.bdl.contracts.integer import ContractInteger
from tools.bdl.contracts.float import ContractFloat
from tools.bdl.contracts.string import ContractString
from tools.bdl.contracts.boolean import ContractBoolean
from tools.bdl.contracts.mandatory import ContractMandatory

_Contracts = [
	ContractInteger(),
	ContractFloat(),
	ContractString(),
	ContractBoolean(),
	ContractMin(),
	ContractMax(),
	ContractTemplate(),
	ContractMandatory()
]


class AllContracts:
	"""Contains all contracts."""

	@cached_classproperty
	def all(cls) -> typing.Mapping[str, ContractTraits]:
		return {contract.name: contract for contract in _Contracts}

	@cached_classproperty
	def forValues(cls) -> typing.Mapping[str, ContractTraits]:
		return {contract.name: contract for contract in _Contracts if contract.isValue}