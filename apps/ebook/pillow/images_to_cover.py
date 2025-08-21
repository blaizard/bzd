import pathlib
import typing
import math

from apps.ebook.flow import ActionInterface, FlowEnum, FlowContext
from apps.ebook.providers import ProviderImages, ProviderCover
from apps.ebook.utils import sizeToString

from PIL import Image


class ImagesToCover(ActionInterface):
	"""Convert the first image of a sequence into a cover image."""

	ProviderInput = ProviderImages
	ProviderOutput = ProviderCover

	def __init__(self, suffix: str = "", area: int = 400 * 600) -> None:
		super().__init__()

		self.suffix = suffix
		self.area = area

	def process(self, provider: ProviderImages, directory: pathlib.Path,
	            context: FlowContext) -> typing.List[typing.Tuple[ProviderCover, typing.Optional[FlowEnum]]]:

		output = directory / f"{provider.metadata.title or 'output'}{self.suffix}.jpg"
		assert len(provider.images) >= 1, "The provider should contain at least 1 image."
		source = provider.images[0]
		with Image.open(source) as image:
			image = image.convert("RGB")
			scale = math.sqrt(self.area / (image.width * image.height))
			width = int(image.width * scale)
			height = int(image.height * scale)
			image = image.resize((width, height), Image.LANCZOS)  # type: ignore
			image.save(output)
			sizeStr = sizeToString(output.stat().st_size)
			print(f"Converting {source.name} to {width}x{height} ({sizeStr}).")

		return [(ProviderCover(path=output), None)]
