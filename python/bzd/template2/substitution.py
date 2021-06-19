import typing
import pathlib
from bzd.parser.element import Element
from bzd.parser.error import Error

SubstitutionsType = typing.MutableMapping[str, typing.Any]


class SubstitutionWrapper:

	def __init__(self, substitutions: SubstitutionsType) -> None:
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
