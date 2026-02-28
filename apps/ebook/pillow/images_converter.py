import pathlib
import typing
from collections import Counter

from apps.ebook.flow import ActionInterface, FlowEnum, FlowContext
from apps.ebook.providers import ProviderImages
from apps.ebook.utils import estimatePageDPIs

from PIL import Image


class ImagesConverter(ActionInterface):
	"""Convert a sequence of images."""

	ProviderInput = ProviderImages
	ProviderOutput = ProviderImages

	def __init__(self, maxDPI: typing.Optional[int] = None, scale: typing.Optional[float] = None) -> None:
		super().__init__()
		self.maxDPI = maxDPI
		self.scale = scale
		assert self.maxDPI is None or self.scale is None, f"Cannot specify both maxDPI and scale, choose one."

	def process(self, provider: ProviderImages, directory: pathlib.Path,
	            context: FlowContext) -> typing.List[typing.Tuple[ProviderImages, typing.Optional[FlowEnum]]]:

		# Precalculate the DPI of the images if necessary.
		dimensions: typing.List[typing.Tuple[int, int]] = []
		for _, currentPath in enumerate(provider.images):
			with Image.open(currentPath) as currentImage:
				width, height = currentImage.size
				dimensions.append((width, height))
		maybeEstimatedDPIs = estimatePageDPIs(dimensions=dimensions)

		images = []
		for index, path in enumerate(provider.images):
			with Image.open(path) as image:
				image = image.convert("RGB")  # type: ignore
				scale = self.scale or 1.0
				comments: typing.List[str] = []

				if self.maxDPI is not None:

					# Get the DPI of this image.
					maybeDPI = image.info.get("dpi", None)
					if maybeEstimatedDPIs is not None:
						pageDPI = maybeEstimatedDPIs[index]
						if maybeDPI is not None:
							ratioDiffXDPI = abs((maybeDPI[0] - pageDPI) / pageDPI)
							ratioDiffYDPI = abs((maybeDPI[1] - pageDPI) / pageDPI)
							if ratioDiffXDPI > 0.1 or ratioDiffYDPI > 0.1:
								comments.append(
								    f"image DPI {int(maybeDPI[0])}x{int(maybeDPI[1])} != estimated DPI {pageDPI}")
								maybeDPI = (pageDPI, pageDPI)
						else:
							maybeDPI = (pageDPI, pageDPI)

					# Calculate new dimensions to achieve DPI.
					if maybeDPI is not None:
						comments.append(f"image DPI {int(maybeDPI[0])}x{int(maybeDPI[1])}")
						if maybeDPI[0] > self.maxDPI or maybeDPI[1] > self.maxDPI:
							scaleX = self.maxDPI / maybeDPI[0]
							scaleY = self.maxDPI / maybeDPI[1]
							scaleDPI = min(scaleX, scaleY)
							assert scaleDPI <= scale, f"Scaling by DPI must be less than or equal to {scale}, got {int(scaleDPI)}."
							if scaleDPI < 0.95 or scaleDPI > 1.05:
								comments.append(f"matching max DPI of {self.maxDPI}")
								scale = scaleDPI
							else:
								scale = 1.0

				# Resize the image if necessary
				assert scale <= 1.0, f"Scale must be less than or equal to 1.0, got {scale}."
				if scale < 1.0:
					path = directory / f"{index}.jpg"
					commentsStr = f": {'; '.join(comments)}" if len(comments) else ""
					print(
					    f"Resizing image '{path.name}' from {image.width}x{image.height} to {int(image.width * scale)}x{int(image.height * scale)} (x{scale:.2f}){commentsStr}."
					)
					image = image.resize((int(image.width * scale), int(image.height * scale)),
					                     Image.LANCZOS)  # type: ignore
					image.save(path)

				images.append(path)

		return [(ProviderImages(images=images, metadata=provider.metadata), None)]
