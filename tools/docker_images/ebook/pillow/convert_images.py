import pathlib
import typing

from tools.docker_images.ebook.flow import ActionInterface, FlowEnum
from tools.docker_images.ebook.providers import ProviderImages


class ConvertImages(ActionInterface):
	"""Convert a sequence of images."""

	ProviderInput = ProviderImages
	ProviderOutput = ProviderImages

	def process(self, provider: ProviderImages,
	            directory: pathlib.Path) -> typing.List[typing.Tuple[ProviderImages, typing.Optional[FlowEnum]]]:

		return [(provider, None)]
