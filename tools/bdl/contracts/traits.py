import typing

from bzd.validation.validation import Validation, SchemaList
from bzd.validation.schema import Constraint

from tools.bdl.contracts.contract import Contract


class Role:
	Value: int = 1
	Template: int = 2
	Meta: int = 4
	Public: int = 8


class ContractTraits:
	"""
    Base class to define a contract.
    """

	def __init__(self,
		name: str,
		role: int,
		validationSchema: SchemaList,
		constraint: typing.Optional[typing.Type[Constraint]] = None) -> None:
		# Name of the contract
		self.name = name
		# Defines contract role
		self.role = role
		# Underlying constraint if any
		self.constraint = constraint
		# Validation object for the contract inputs
		self.validation = Validation(validationSchema)

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

	def validate(self, contract: Contract) -> None:
		"""
        Validate the inputs of a contract. For example, validates that min(1) contains a single argument "1"
		and that it is a number.
		TODO: remove this check from here, it should be checked within the validation constraint.
        """
		self.validation.validate(contract.values)

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
		raise Exception("cannot be merged.")
