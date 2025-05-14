import pathlib
import typing
import dataclasses
from apps.ebook.providers import ProviderEbookMetadata


@dataclasses.dataclass
class EPubMetadata:
	# Ordered list of documents associated to this ebook.
	document: typing.List[pathlib.Path] = dataclasses.field(default_factory=list)
	# The metadata associated with this ebook.
	metadata: ProviderEbookMetadata = dataclasses.field(default_factory=ProviderEbookMetadata)

	def clone(self) -> "EPubMetadata":
		return dataclasses.replace(self)
