"""Inputs and Outputs of the various flow elements."""

import pathlib
import typing
import dataclasses
import json

Json = typing.Dict[str, typing.Any]


@dataclasses.dataclass
class ProviderEbook:
	"""Provides an ebook as a file."""

	# Path of the input ebook.
	ebook: pathlib.Path
	# Keys associated to this ebook.
	keys: typing.List[pathlib.Path] = dataclasses.field(default_factory=list)

	@property
	def hasKeys(self) -> bool:
		"""Check if this ebook as associated keys."""
		return len(self.keys) > 0

	def toJson(self) -> Json:
		return {"ebook": str(self.ebook), "keys": [str(key) for key in self.keys]}

	@staticmethod
	def fromJson(data: Json) -> "ProviderEbook":
		return ProviderEbook(ebook=pathlib.Path(data["ebook"]), keys=[pathlib.Path(key) for key in data["keys"]])


@dataclasses.dataclass
class ProviderImages:
	"""Provides a sequence of images."""

	# Ordered sequence of images.
	images: typing.List[pathlib.Path]

	def toJson(self) -> Json:
		return {"images": [str(image) for image in self.images]}

	@staticmethod
	def fromJson(data: Json) -> "ProviderImages":
		return ProviderImages(images=[pathlib.Path(image) for image in data["images"]])


Provider = typing.Union[ProviderEbook, ProviderImages]


def providerSerialize(provider: Provider) -> str:
	return json.dumps({"type": provider.__class__.__name__, "values": provider.toJson()})


def providerDeserialize(data: str) -> Provider:
	content = json.loads(data)
	for providerType in typing.get_args(Provider):
		if providerType.__name__ == content["type"]:
			return providerType.fromJson(content["values"])  # type: ignore
	raise Exception(f"Cannot deserialize the provider of type: '{content['type']}'.")
