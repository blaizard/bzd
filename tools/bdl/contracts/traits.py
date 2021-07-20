import typing

from bzd.validation.validation import Validation, SchemaList
from tools.bdl.contracts.contract import Contract


class ContractTraits:
	"""
    Base class to define a contract.
    """

	def __init__(self, name: str, isValue: bool, validationSchema: SchemaList) -> None:
		# Name of the contract
		self.name = name
		# Defines contract targeting values
		self.isValue = isValue
		# Validation object for the ccontract inputs
		self.validation = Validation(validationSchema)

	def validate(self, contract: Contract) -> None:
		"""
        Validate the inputs of a contract.
        """
		self.validation.validate(contract.values)

	def resolveConflict(self, base: Contract, derived: Contract) -> typing.Optional[Contract]:
		"""
        Used to resolve a conflict between 2 contracts.
        """
		try:
			return self.resolveConflictInternals(base, derived)
		except Exception as e:
			raise Exception("Conflict with contracts '{}' and '{}': {}".format(str(base), str(derived), str(e)))

	def resolveConflictInternals(self, base: Contract, derived: Contract) -> typing.Optional[Contract]:
		"""
        Overload to resolve conflicts.
        """
		raise Exception("cannot be merged.")
