import pathlib
import typing

from tools.docker_images.ebook.flow import ActionInterface, FlowEnum
from tools.docker_images.ebook.providers import ProviderImages

from PIL import Image


class ImagesToPdf(ActionInterface):
	"""Convert a sequence of images into a PDF."""

	ProviderInput = ProviderImages
	ProviderOutput = ProviderImages

	def process(self, provider: ProviderImages,
	            directory: pathlib.Path) -> typing.List[typing.Tuple[ProviderImages, typing.Optional[FlowEnum]]]:

		output = directory / "output.pdf"
		print(f"Converting {len(provider.images)} images into {output}.")

		images = []
		for index, path in enumerate(provider.images):
			image = Image.open(path)
			images.append(image.convert("RGB"))
		images[0].save(output, save_all=True, append_images=images[1:])

		return [(provider, None)]
