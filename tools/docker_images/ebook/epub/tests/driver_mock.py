import pathlib
import typing

from tools.docker_images.ebook.epub.metadata import EPubMetadata


class DriverMock:
	"""Mock driver to extract pages."""

	def __init__(self, expected: typing.List[EPubMetadata]) -> None:
		self.expected = expected
		self.currentlyExpected: typing.List[EPubMetadata] = []

	def __enter__(self) -> "DriverMock":
		self.currentlyExpected = [expectedMetadata.clone() for expectedMetadata in self.expected]
		return self

	def process(self, documentIndex: int, metadata: EPubMetadata, directory: pathlib.Path) -> typing.List[pathlib.Path]:

		self.currentlyExpected = [expectedMetadata.clone() for expectedMetadata in self.expected]

		assert len(self.currentlyExpected) > documentIndex, f"There is no expected document at index {documentIndex}."
		expected = self.currentlyExpected[documentIndex]

		for document in metadata.document:
			assert len(expected.document
			           ) > 0, f"There are more documents than what is expected, extra document: '{document.name}'."
			expectedDocument = expected.document.pop(0)
			assert str(
			    expectedDocument
			) == document.name, f"Expected document '{expectedDocument}' differ from actual '{document.name}'."

		for attrName in ["identifier", "title", "language", "creators", "contributors", "date", "publisher"]:
			attr = getattr(expected, attrName)
			if not attr:
				continue
			actual = getattr(metadata, attrName)
			assert attr == actual, f"Attribute '{attrName}' differs, expected '{attr}' vs actual '{actual}'."

		return []

	def __exit__(self, *args: typing.Any) -> None:
		for documentIndex, expected in enumerate(self.currentlyExpected):
			assert len(
			    expected.document
			) == 0, f"Some pages in document {documentIndex} were expected but not found: {str(expected.document)}"
