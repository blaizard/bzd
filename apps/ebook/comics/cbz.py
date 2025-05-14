import pathlib
import typing
import zipfile

from apps.ebook.comics.comics import Comics
from apps.ebook.flow import FlowEnum
from apps.ebook.providers import ProviderEbook, ProviderImages


class Cbz(Comics):
	"""Convert a .cbz file into a sequence of images."""

	def process(self, provider: ProviderEbook,
	            directory: pathlib.Path) -> typing.List[typing.Tuple[ProviderImages, typing.Optional[FlowEnum]]]:

		with zipfile.ZipFile(str(provider.ebook), "r") as zipInstance:
			zipInstance.extractall(directory / "extract")

		return self.processDirectory(provider, directory / "extract")
