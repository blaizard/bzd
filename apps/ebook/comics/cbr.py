import pathlib
import typing
import rarfile

from apps.ebook.comics.comics import Comics
from apps.ebook.flow import FlowEnum
from apps.ebook.providers import ProviderEbook, ProviderImages


class Cbr(Comics):
	"""Convert a .cbr file into a sequence of images."""

	def process(self, provider: ProviderEbook,
	            directory: pathlib.Path) -> typing.List[typing.Tuple[ProviderImages, typing.Optional[FlowEnum]]]:

		with rarfile.RarFile(str(provider.ebook), "r") as rarInstance:  # type: ignore
			rarInstance.extractall(directory / "extract")

		return self.processDirectory(provider, directory / "extract")
