import pathlib
import typing
import dataclasses


@dataclasses.dataclass
class EPubMetadata:
	# Ordered list of documents associated to this ebook.
	document: typing.List[pathlib.Path] = dataclasses.field(default_factory=list)
	# The identifier of this document.
	identifier: typing.Optional[str] = None
	# The title of this document.
	title: typing.Optional[str] = None
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

	def clone(self) -> "EPubMetadata":
		return dataclasses.replace(self)
