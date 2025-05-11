"""Inputs and Outputs of the various flow elements."""

import pathlib
import typing
import dataclasses
import json

Json = typing.Dict[str, typing.Any]


@dataclasses.dataclass
class ProviderEbookMetadata:
	"""Provides the ebook metadata."""

	# The title of this document.
	title: typing.Optional[str] = None
	# The identifier of this document.
	identifier: typing.Optional[str] = None
	# The language for this document.
	language: typing.Optional[str] = None
	# List of creators.
	creators: typing.List[str] = dataclasses.field(default_factory=list)
	# List of contributors that play a secondary role for this ebook.
	contributors: typing.List[str] = dataclasses.field(default_factory=list)
	# Publication date.
	date: typing.Optional[str] = None
	# Publisher name.
	publisher: typing.Optional[str] = None

	def toJson(self) -> Json:
		return dataclasses.asdict(self)

	@staticmethod
	def fromJson(data: Json) -> "ProviderEbookMetadata":
		return ProviderEbookMetadata(**data)


@dataclasses.dataclass
class ProviderEbook:
	"""Provides an ebook as a file."""

	# Path of the input ebook.
	ebook: pathlib.Path
	# Keys associated to this ebook.
	keys: typing.List[pathlib.Path] = dataclasses.field(default_factory=list)
	# Metadata associated with this ebook.
	metadata: ProviderEbookMetadata = dataclasses.field(default_factory=ProviderEbookMetadata)

	@property
	def hasKeys(self) -> bool:
		"""Check if this ebook as associated keys."""
		return len(self.keys) > 0

	def toJson(self) -> Json:
		return {"ebook": str(self.ebook), "keys": [str(key) for key in self.keys], "metadata": self.metadata.toJson()}

	@staticmethod
	def fromJson(data: Json) -> "ProviderEbook":
		return ProviderEbook(ebook=pathlib.Path(data["ebook"]),
		                     keys=[pathlib.Path(key) for key in data["keys"]],
		                     metadata=ProviderEbookMetadata.fromJson(data["metadata"]))


@dataclasses.dataclass
class ProviderImages:
	"""Provides a sequence of images."""

	# Ordered sequence of images.
	images: typing.List[pathlib.Path]
	# Metadata associated with this ebook.
	metadata: ProviderEbookMetadata = dataclasses.field(default_factory=ProviderEbookMetadata)

	def toJson(self) -> Json:
		return {"images": [str(image) for image in self.images], "metadata": self.metadata.toJson()}

	@staticmethod
	def fromJson(data: Json) -> "ProviderImages":
		return ProviderImages(images=[pathlib.Path(image) for image in data["images"]],
		                      metadata=ProviderEbookMetadata.fromJson(data["metadata"]))


Provider = typing.Union[ProviderEbook, ProviderImages]


def providerSerialize(provider: Provider) -> str:
	return json.dumps({"type": provider.__class__.__name__, "values": provider.toJson()})


def providerDeserialize(data: str) -> Provider:
	content = json.loads(data)
	for providerType in typing.get_args(Provider):
		if providerType.__name__ == content["type"]:
			return providerType.fromJson(content["values"])  # type: ignore
	raise Exception(f"Cannot deserialize the provider of type: '{content['type']}'.")
