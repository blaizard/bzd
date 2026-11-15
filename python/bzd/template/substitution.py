import typing

from bzd.parser.element import Element
from bzd.template.builtins import Builtins

SubstitutionsType = typing.Any


class SubstitutionsAccessor:
	def __init__(self, *args: SubstitutionsType) -> None:
		self.substitutionsList: list[SubstitutionsType] = [*args]

	def __getitem__(self, key: str) -> typing.Any:
		for substitutions in self.substitutionsList:
			if hasattr(substitutions, key):
				return getattr(substitutions, key)
			elif hasattr(substitutions, "__contains__") and key in substitutions:
				return substitutions[key]
		raise KeyError(f"Key '{key}' does not exists.")

	def __contains__(self, key: str) -> bool:
		for substitutions in self.substitutionsList:
			if hasattr(substitutions, key) or hasattr(substitutions, "__contains__") and key in substitutions:
				return True
		return False

	def get(self, key: str, default: typing.Any = None) -> typing.Any:
		if key in self:
			return self[key]
		return default

	def __repr__(self) -> str:
		content = "\n".join(["\t" + str(substitutions).split("\n")[0] for substitutions in self.substitutionsList])
		return f"<SubstitutionsAccessor>\n{content}\n</SubstitutionsAccessor>"


class SubstitutionWrapper:
	def __init__(self, substitutions: SubstitutionsAccessor) -> None:
		self.substitutions = substitutions
		self.ext: dict[str, list[typing.Any]] = {}

	def register(self, element: Element, key: str, value: typing.Any) -> None:
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
		ext = self.ext.get(key)
		if ext:
			return ext[-1]
		if key in self.substitutions:
			return self.substitutions[key]
		return Builtins[key]

	def __contains__(self, key: str) -> bool:
		if key in self.substitutions:
			return True
		return bool(self.ext.get(key))

	def get(self, key: str, default: typing.Any = None) -> typing.Any:
		if key in self:
			return self[key]
		return default

	def __repr__(self) -> str:
		content = "\n".join([f"\t{substitution}" for substitution in str(self.substitutions).split("\n")])
		return f"<SubstitutionWrapper>\n{content}\n\t<Extensions {', '.join([k for k in self.ext])}/>\n</SubstitutionWrapper>"
