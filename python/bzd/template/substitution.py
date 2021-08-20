import typing
import pathlib
from bzd.parser.element import Element
from bzd.parser.error import Error

SubstitutionsType = typing.Any


class SubstitutionsAccessor:

	def __init__(self, *args: SubstitutionsType) -> None:
		self.substitutionsList: typing.List[SubstitutionsType] = [*args]

	def __getitem__(self, key: str) -> typing.Any:
		for substitutions in self.substitutionsList:
			if hasattr(substitutions, key):
				return getattr(substitutions, key)
			elif hasattr(substitutions, "__contains__") and key in substitutions:
				return substitutions[key]
		raise KeyError("Key '{}' does not exists.".format(key))

	def __contains__(self, key: str) -> bool:
		for substitutions in self.substitutionsList:
			if hasattr(substitutions, key):
				return True
			elif hasattr(substitutions, "__contains__") and key in substitutions:
				return True
		return False


class SubstitutionWrapper:

	def __init__(self, substitutions: SubstitutionsAccessor) -> None:
		self.substitutions = substitutions
		self.ext: typing.Dict[str, typing.List[typing.Any]] = {}

	def register(self, element: Element, key: str, value: typing.Any) -> None:
		Error.assertTrue(element=element,
			condition=(not self.isInSubstitutions(key)),
			message="Name conflict, '{}' already exists in the substitution map.".format(key))
		if key not in self.ext:
			self.ext[key] = []
		self.ext[key].append(value)

	def unregister(self, key: str) -> None:
		self.ext[key].pop()

	def update(self, other: "SubstitutionWrapper") -> None:
		"""
		Update the current object with another substitution wrapper.
		This will only update the ext part.
		"""
		for key, value in other.ext.items():
			if value:
				if key not in self.ext:
					self.ext[key] = []
				self.ext[key].append(value[-1])

	def __getitem__(self, key: str) -> typing.Any:
		if hasattr(self.substitutions, key):
			return getattr(self.substitutions, key)
		elif key in self.substitutions:
			return self.substitutions[key]
		return self.ext[key][-1]

	def isInSubstitutions(self, key: str) -> bool:
		"""
		If the element is in the original subsitution map.
		"""
		return hasattr(self.substitutions, key) or key in self.substitutions

	def __contains__(self, key: str) -> bool:
		return self.isInSubstitutions(key) or key in self.ext
