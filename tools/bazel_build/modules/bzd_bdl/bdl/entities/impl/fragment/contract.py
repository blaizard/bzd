import typing

from bzd.parser.element import Element, ElementBuilder, SequenceBuilder
from bzd.parser.error import Error

from bdl.contracts.validation import Validation
from bdl.contracts.contract import Contract
from bdl.contracts.all import AllContracts


class Contracts:
	"""
    A contract is an unsorted sequence of types associated with an optional value and/or comment.
    """

	def __init__(self, element: Element, sequenceKind: typing.Optional[str] = None) -> None:
		self.element = element
		self.sequenceKind = "contract" if sequenceKind is None else sequenceKind

	def get(self, kind: str) -> typing.Optional[Contract]:
		"""
        Get a specific contract type if present.
        """
		for contract in self:
			if contract.type == kind:
				return contract
		return None

	def has(self, kind: str) -> bool:
		for contract in self:
			if contract.type == kind:
				return True
		return False

	def assertOnly(self, kinds: typing.Set[str]) -> None:
		"""Assert that only the contract kinds passed into argument are available in this one."""

		for contract in self:
			Error.assertTrue(
			    condition=contract.type in kinds,
			    element=self.element,
			    message=
			    f"The contract type '{contract.type}' is not allowed for this entity, only the following(s) are supported: {', '.join(kinds)}.",
			)

	@property
	def validationForAll(self) -> typing.Optional[str]:
		"""
        Generate a validation string for all contracts
        """
		content = [str(contract) for contract in self]
		return " ".join(content) if content else None

	@property
	def validationForValue(self) -> typing.Optional[str]:
		"""
        Generate a validation string for a value out of the current contracts
        """
		content = [str(contract) for contract in self if contract.type in AllContracts.forValue]
		return " ".join(content) if content else None

	@property
	def validationForTemplate(self) -> typing.Optional[str]:
		"""
        Generate a validation string for a template out of the current contracts
        """
		content = [str(contract) for contract in self if contract.type in AllContracts.forTemplate]
		return " ".join(content) if content else None

	@property
	def validationForEntity(self) -> typing.Optional[str]:
		"""
        Generate a validation string for an entity out of the current contracts
        """
		content = [str(contract) for contract in self if contract.type in AllContracts.forEntity]
		return " ".join(content) if content else None

	@property
	def empty(self) -> bool:
		sequence = self.element.getNestedSequence(self.sequenceKind)
		if sequence is None:
			return True
		return len(sequence) == 0

	def validate(self) -> None:
		"""
        Validate the contracts. it checks the follow:
                - Valid contract types.
        """

		for contract in self:
			contractTraits = AllContracts.allPublic.get(contract.type)
			if contractTraits is None:
				Error.handleFromElement(
				    element=self.element,
				    message="Contract of type '{}' is not supported.".format(contract.type),
				)

	def merge(self, contracts: "Contracts") -> None:
		"""
        Merge a base contract into this one. The order is important, as type are inherited from the deepest base.
        """

		elementBuilder = ElementBuilder.cast(self.element, ElementBuilder)
		for contract in reversed([*contracts]):
			# Check if the type already exists, if so attempt to merge the 2 types.
			existing = self.get(contract.type)
			if existing is not None:
				contractTraits = AllContracts.all.get(contract.type)
				assert (contractTraits), "All contracts should have been validated already."
				try:
					contract = contractTraits.resolveConflict(base=contract, derived=existing)
				except Exception as e:
					Error.handleFromElement(element=self.element, message=str(e),
					                        throw=False).extend(element=contracts.element,
					                                            message="Base contract is here.")
				# Remove the existing contract if resolved into a new contract
				if contract is not None:
					self.remove(contract.type)

			if contract is not None:
				elementBuilder.pushFrontElementToNestedSequence(self.sequenceKind, contract.element)

		# Construct a dummy Validation to check the arguments
		maybeSchema = self.validationForAll
		if maybeSchema:
			Validation(schema=[maybeSchema])

	def remove(self, kind: str) -> None:
		"""
        Remove contracts of a specified type.
        """
		sequence = self.element.getNestedSequence(self.sequenceKind)
		if sequence:
			for element in sequence:
				if Contract(element).type == kind:
					SequenceBuilder.cast(sequence, SequenceBuilder).removeElement(element)

	def __iter__(self) -> typing.Iterator[Contract]:
		sequence = self.element.getNestedSequence(self.sequenceKind)
		if sequence:
			for element in sequence:
				yield Contract(element)

	def __repr__(self) -> str:
		return " ".join([str(contract) for contract in self])
