import typing

from bzd.utils.decorators import cached_classproperty

from bdl.contracts.traits import ContractTraits
from bdl.contracts.min import ContractMin
from bdl.contracts.max import ContractMax
from bdl.contracts.builtins import ContractInteger, ContractFloat, ContractBoolean
from bdl.contracts.string import ContractString
from bdl.contracts.mandatory import ContractMandatory
from bdl.contracts.capacity import ContractCapacity
from bdl.contracts.init import ContractInit
from bdl.contracts.shutdown import ContractShutdown
from bdl.contracts.convertible import ContractConvertible
from bdl.contracts.executor import ContractExecutor

_Contracts = [
    ContractInteger(),
    ContractFloat(),
    ContractString(),
    ContractBoolean(),
    ContractMin(),
    ContractMax(),
    ContractMandatory(),
    ContractCapacity(),
    ContractInit(),
    ContractShutdown(),
    ContractConvertible(),
    ContractExecutor(),
]


class AllContracts:
	"""Contains all contracts."""

	@cached_classproperty
	def all(cls) -> typing.Mapping[str, ContractTraits]:
		return {contract.name: contract for contract in _Contracts}

	@cached_classproperty
	def allPublic(cls) -> typing.Mapping[str, ContractTraits]:
		return {contract.name: contract for contract in _Contracts if contract.isRolePublic}

	@cached_classproperty
	def forValue(cls) -> typing.Mapping[str, ContractTraits]:
		return {contract.name: contract for contract in _Contracts if contract.isRoleValue}

	@cached_classproperty
	def forTemplate(cls) -> typing.Mapping[str, ContractTraits]:
		return {contract.name: contract for contract in _Contracts if contract.isRoleTemplate}

	@cached_classproperty
	def forEntity(cls) -> typing.Mapping[str, ContractTraits]:
		return {contract.name: contract for contract in _Contracts if contract.isRoleEntity}
