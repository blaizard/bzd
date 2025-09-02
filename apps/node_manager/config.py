import pathlib
import json
import typing
import re
from dataclasses import dataclass


@dataclass
class UPSClient:
	hostname: str = "127.0.0.1"
	port: int = 3493
	username: typing.Optional[str] = None
	password: typing.Optional[str] = None


class Config:
	"""Accessor for the config.
	
	The configuration looks like this:
	{
		disks: {
			mappings: {
				".*/volume2.*": "/volume2",
				".*/photo.*/video.*": "/volume1"
			}
		}
	}
	"""

	def __init__(self, path: typing.Optional[pathlib.Path] = None) -> None:
		self.config: typing.Dict[str, typing.Any] = {}
		if path is not None:
			self.config = json.loads(path.read_text())
		self.preprocess()

	def preprocess(self) -> None:
		"""Preprocess the configuration."""

		self.preprocessed: typing.Dict[str, typing.Any] = {}

		# Preprocess the mapping for the disks. Ensure the order
		# is reproductible by sorting the keys.
		mapping = self.config.get("disks", {}).get("mappings", {})
		newMapping = []
		for regexpr, identifier in sorted(mapping.items()):
			newMapping.append((re.compile(regexpr), identifier))
		self.preprocessed["disks.mappings"] = newMapping

	def disksMappings(self, dictionary: typing.Dict[str, typing.Any]) -> typing.Dict[str, typing.Any]:
		"""Convert a disk identifier to a custom identifier if set by the configuration.
		
		Otherwise return the original identifier.
		"""

		mapping = self.config.get("disks", {}).get("mappings", {})

		# Look for the first match per replacement, this to handle duplicates in a reproducible manner.
		matchesPerReplacement: typing.Dict[str, typing.List[str]] = {}
		for identifier in sorted(dictionary.keys()):
			for regexpr, replacement in self.preprocessed["disks.mappings"]:
				if regexpr.fullmatch(identifier):
					matchesPerReplacement.setdefault(replacement, []).append(identifier)
					break

		# Create the dictionary for matches only.
		matches: typing.Dict[str, str] = {}
		for replacement, identifiers in matchesPerReplacement.items():
			if len(identifiers) == 1:
				matches[identifiers[0]] = replacement
			else:
				for index, identifier in enumerate(identifiers):
					matches[identifier] = f"{replacement}.{index}"

		return {matches.get(k, k): v for k, v in dictionary.items()}

	def ups(self) -> typing.Dict[str, UPSClient]:
		"""Get all the ups clients."""

		ups = {name: UPSClient(**data) for name, data in self.config.get("ups", {}).items()}
		# If not client is explicitly set, look if there is a default NUT server.
		return ups if ups else {"": UPSClient()}
