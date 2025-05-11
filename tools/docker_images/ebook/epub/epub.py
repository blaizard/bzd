import enum
import glob
import json
import pathlib
import shutil
import subprocess
import tempfile
import typing
import zipfile
import xml.etree.ElementTree as ET

from tools.docker_images.ebook.epub.driver_selenium import DriverSelenium
from tools.docker_images.ebook.epub.metadata import EPubMetadata
from tools.docker_images.ebook.flow import ActionInterface, FlowEnum
from tools.docker_images.ebook.providers import ProviderEbook, ProviderImages


class EPub(ActionInterface):
	"""Flow for getting images from epub files."""

	ProviderInput = ProviderEbook
	ProviderOutput = ProviderImages

	def __init__(self, driver: typing.Any = DriverSelenium()) -> None:
		super().__init__()
		self.driver = driver

	def readDC(self, root: ET.Element) -> EPubMetadata:
		"""Populate the metadata."""

		metadata = EPubMetadata()
		for elt in root.iter():
			if elt.tag.endswith("}title"):
				metadata.title = elt.text
			elif elt.tag.endswith("}identifier"):
				metadata.identifier = elt.text
			elif elt.tag.endswith("}language"):
				metadata.language = elt.text
			elif elt.tag.endswith("}creator"):
				assert elt.text is not None
				metadata.creators.append(elt.text)
			elif elt.tag.endswith("}contributor"):
				assert elt.text is not None
				metadata.contributors.append(elt.text)
			elif elt.tag.endswith("}date"):
				metadata.date = elt.text
			elif elt.tag.endswith("}publisher"):
				metadata.publisher = elt.text
		return metadata

	def readMetadata(self, dirname: pathlib.Path) -> typing.List[EPubMetadata]:
		"""Read the metadata from an epub."""

		tree = ET.parse(dirname / "META-INF/container.xml")
		root = tree.getroot()
		rootfiles = [elt.attrib["full-path"] for elt in root.iter() if elt.tag.endswith("rootfile")]
		documents: typing.List[EPubMetadata] = []
		for rootfile in rootfiles:
			tree = ET.parse(dirname / rootfile)
			root = tree.getroot()

			metadata = self.readDC(root)

			# Create a map of id -> path.
			rootDirname = (dirname / rootfile).parent
			manifest = {
			    entry.attrib["id"]: rootDirname / entry.attrib["href"]
			    for manifest in [elt for elt in root.iter() if elt.tag.endswith("manifest")]
			    for entry in manifest
			}

			for spine in [elt for elt in root.iter() if elt.tag.endswith("spine")]:
				documentMetadata = metadata.clone()
				documentMetadata.document = [manifest[item.attrib["idref"]] for item in spine]
				documents.append(documentMetadata)

		return documents

	def process(self, provider: ProviderEbook,
	            directory: pathlib.Path) -> typing.List[typing.Tuple[ProviderImages, typing.Optional[FlowEnum]]]:

		outputs = []
		with self.driver as driver:
			with zipfile.ZipFile(str(provider.ebook), "r") as zipInstance:
				zipInstance.extractall(directory / "extract")
			documents = self.readMetadata(directory / "extract")
			for documentNumber, metadata in enumerate(documents):
				output = driver.process(documentNumber, metadata, directory / f"document_{documentNumber}")
				print(output)
				outputs += output

		return [(ProviderImages(images=outputs), None)]
