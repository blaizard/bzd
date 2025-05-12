import pathlib
import typing

from tools.docker_images.ebook.flow import ActionInterface, FlowEnum
from tools.docker_images.ebook.providers import ProviderImages, ProviderPdf

from PIL import Image


class ImagesToPdf(ActionInterface):
	"""Convert a sequence of images into a PDF."""

	ProviderInput = ProviderImages
	ProviderOutput = ProviderPdf

	def process(self, provider: ProviderImages,
	            directory: pathlib.Path) -> typing.List[typing.Tuple[ProviderPdf, typing.Optional[FlowEnum]]]:

		output = directory / f"{provider.metadata.title or 'output'}.pdf"
		print(f"Converting {len(provider.images)} images into {output}.")

		images = []
		for index, path in enumerate(provider.images):
			image = Image.open(path)
			images.append(image.convert("RGB"))
		metadata = {}
		if provider.metadata.title:
			metadata["title"] = provider.metadata.title
		if len(provider.metadata.creators) > 0:
			metadata["author"] = ", ".join(provider.metadata.creators)
		if provider.metadata.date:
			metadata["creationDate"] = provider.metadata.date.strftime("D:%Y%m%d%H%M%S%z")

		assert len(images) > 0, f"There must be at least one image from the ebook."
		images[0].save(output, save_all=True, append_images=images[1:], **metadata)

		return [(ProviderPdf(path=output), None)]
