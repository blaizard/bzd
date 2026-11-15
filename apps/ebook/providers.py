"""Inputs and Outputs of the various flow elements."""

import dataclasses
import datetime
import hashlib
import json
import pathlib
import typing

Json = dict[str, typing.Any]


class ProviderBase:
	"""Base class for a provider."""

	def __init__(self) -> None:
		assert hasattr(self, "toJson"), "The 'toJson' must be implemented."
		assert hasattr(self, "fromJson"), "The 'fromJson' must be implemented."

	def hash(self) -> str:
		string = json.dumps(self.toJson())  # type: ignore
		return str(hashlib.sha256(string.encode("utf-8")).hexdigest())


@dataclasses.dataclass
class ProviderEbookMetadata(ProviderBase):
	"""Provides the ebook metadata."""

	# The title of this document.
	title: str | None = None
	# The identifier of this document.
	identifier: str | None = None
	# The language for this document.
	language: str | None = None
	# List of creators.
	creators: list[str] = dataclasses.field(default_factory=list)
	# List of contributors that play a secondary role for this ebook.
	contributors: list[str] = dataclasses.field(default_factory=list)
	# Publication date.
	date: datetime.datetime | None = None
	# Publisher name.
	publisher: str | None = None

	def toJson(self) -> Json:
		return {
			"title": self.title,
			"identifier": self.identifier,
			"language": self.language,
			"creators": self.creators,
			"contributors": self.contributors,
			"date": None if self.date is None else self.date.isoformat(),
			"publisher": self.publisher,
		}

	@staticmethod
	def fromJson(data: Json) -> "ProviderEbookMetadata":
		return ProviderEbookMetadata(
			title=data["title"],
			identifier=data["identifier"],
			language=data["language"],
			creators=data["creators"],
			contributors=data["contributors"],
			date=datetime.datetime.fromisoformat(data["date"]) if data["date"] else None,
			publisher=data["publisher"],
		)


@dataclasses.dataclass
class ProviderEbook(ProviderBase):
	"""Provides an ebook as a file."""

	# Path of the input ebook.
	ebook: pathlib.Path
	# Keys associated to this ebook.
	keys: list[pathlib.Path] = dataclasses.field(default_factory=list)
	# Metadata associated with this ebook.
	metadata: ProviderEbookMetadata = dataclasses.field(default_factory=ProviderEbookMetadata)

	def clone(self) -> "ProviderEbook":
		"""Create a new ebook provider from an existing one."""
		return dataclasses.replace(self)

	@property
	def hasKeys(self) -> bool:
		"""Check if this ebook as associated keys."""
		return len(self.keys) > 0

	def toJson(self) -> Json:
		return {
			"ebook": str(self.ebook),
			"keys": [str(key) for key in self.keys],
			"metadata": self.metadata.toJson(),
		}

	@staticmethod
	def fromJson(data: Json) -> "ProviderEbook":
		return ProviderEbook(
			ebook=pathlib.Path(data["ebook"]),
			keys=[pathlib.Path(key) for key in data["keys"]],
			metadata=ProviderEbookMetadata.fromJson(data["metadata"]),
		)


@dataclasses.dataclass
class ProviderImages(ProviderBase):
	"""Provides a sequence of images."""

	# Ordered sequence of images.
	images: list[pathlib.Path]
	# Metadata associated with this ebook.
	metadata: ProviderEbookMetadata = dataclasses.field(default_factory=ProviderEbookMetadata)

	def toJson(self) -> Json:
		return {
			"images": [str(image) for image in self.images],
			"metadata": self.metadata.toJson(),
		}

	@staticmethod
	def fromJson(data: Json) -> "ProviderImages":
		return ProviderImages(
			images=[pathlib.Path(image) for image in data["images"]],
			metadata=ProviderEbookMetadata.fromJson(data["metadata"]),
		)


@dataclasses.dataclass
class ProviderPdf(ProviderBase):
	"""The final provider for a pdf."""

	# The path of the pdf.
	path: pathlib.Path

	def toJson(self) -> Json:
		return {"path": str(self.path)}

	@staticmethod
	def fromJson(data: Json) -> "ProviderPdf":
		return ProviderPdf(path=pathlib.Path(data["path"]))

	@property
	def outputs(self) -> list[pathlib.Path]:
		"""Return the outputs of this provider."""
		return [self.path]


@dataclasses.dataclass
class ProviderCover(ProviderBase):
	"""The final provider for a cover image."""

	# The path of the cover image.
	path: pathlib.Path

	def toJson(self) -> Json:
		return {"path": str(self.path)}

	@staticmethod
	def fromJson(data: Json) -> "ProviderCover":
		return ProviderCover(path=pathlib.Path(data["path"]))

	@property
	def outputs(self) -> list[pathlib.Path]:
		"""Return the outputs of this provider."""
		return [self.path]


Provider = ProviderEbook | ProviderImages | ProviderPdf | ProviderCover


def providerSerialize(provider: Provider) -> str:
	return json.dumps({"type": provider.__class__.__name__, "values": provider.toJson()})


def providerDeserialize(data: str) -> Provider:
	content = json.loads(data)
	for providerType in typing.get_args(Provider):
		if providerType.__name__ == content["type"]:
			return providerType.fromJson(content["values"])  # type: ignore
	raise RuntimeError(f"Cannot deserialize the provider of type: '{content['type']}'.")
