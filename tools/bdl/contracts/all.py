import typing

from bzd.utils.decorators import cached_classproperty

from tools.bdl.contracts.traits import ContractTraits
from tools.bdl.contracts.min import ContractMin
from tools.bdl.contracts.max import ContractMax
from tools.bdl.contracts.integer import ContractInteger
from tools.bdl.contracts.float import ContractFloat
from tools.bdl.contracts.string import ContractString
from tools.bdl.contracts.boolean import ContractBoolean
from tools.bdl.contracts.mandatory import ContractMandatory
from tools.bdl.contracts.capacity import ContractCapacity
from tools.bdl.contracts.init import ContractInit
from tools.bdl.contracts.convertible import ContractConvertible

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
	ContractConvertible()
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
