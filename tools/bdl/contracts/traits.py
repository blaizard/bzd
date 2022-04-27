import typing

from bzd.validation.schema import Constraint

from tools.bdl.contracts.contract import Contract


class Role:
	"""Define the type of entity to which applies this constraint."""

	# Applies to entity of role Value
	Value: typing.Final[int] = 1
	# Applies to entity of role Template
	Template: typing.Final[int] = 2
	Meta: typing.Final[int] = 4
	# Can be set by the user
	Public: typing.Final[int] = 8
	# Applies this contract to an entity
	Entity: typing.Final[int] = 16


class ContractTraits:
	"""
    Base class to define a contract.
    """

	def __init__(self, name: str, role: int, constraint: typing.Optional[typing.Type[Constraint]] = None) -> None:
		# Name of the contract
		self.name = name
		# Defines contract role
		self.role = role
		# Underlying constraint if any
		self.constraint = constraint

	@property
	def isRoleValue(self) -> bool:
		return bool(self.role & Role.Value)

	@property
	def isRoleTemplate(self) -> bool:
		return bool(self.role & Role.Template)

	@property
	def isRoleMeta(self) -> bool:
		return bool(self.role & Role.Meta)

	@property
	def isRolePublic(self) -> bool:
		return bool(self.role & Role.Public)

	@property
	def isRoleEntity(self) -> bool:
		return bool(self.role & Role.Entity)

	def resolveConflict(self, base: Contract, derived: Contract) -> typing.Optional[Contract]:
		"""
        Used to resolve a conflict between 2 contracts.
        """
		try:
			return self.resolveConflictInternals(base, derived)
		except Exception as e:
			raise Exception("Conflict with base contract [{}] and derived [{}]: {}".format(
				str(base), str(derived), str(e)))

	def resolveConflictInternals(self, base: Contract, derived: Contract) -> typing.Optional[Contract]:
		"""
        Overload to resolve conflicts.
        """
		if base == derived:
			return base
		raise Exception("cannot be merged.")
