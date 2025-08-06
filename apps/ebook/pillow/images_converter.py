import pathlib
import typing
from collections import Counter

from apps.ebook.flow import ActionInterface, FlowEnum
from apps.ebook.providers import ProviderImages

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

	@staticmethod
	def clusteringDimensions(dimensions: typing.List[typing.Tuple[int, int]],
	                         tolerance: float) -> typing.List[typing.List[typing.Tuple[int, int]]]:
		"""Cluster dimensions using the max tolerance as deviation."""

		# Sort the items. Sorting by width primarily and then by height
		# helps to process items that are "close" in both dimensions together.
		# This is important for the greedy grouping strategy.
		sortedDimensions = sorted(dimensions, key=lambda x: (x[0], x[1]))

		clusters = []
		currentCluster: typing.Optional[typing.List[typing.Tuple[int, int]]] = None
		maxAllowedWidth: float = 0
		maxAllowedHeight: float = 0
		for width, height in sortedDimensions:

			if currentCluster is not None:
				if width <= maxAllowedWidth and height <= maxAllowedHeight:
					currentCluster.append((width, height))
				else:
					currentCluster = None

			if currentCluster is None:
				currentCluster = []
				clusters.append(currentCluster)
				currentCluster.append((width, height))
				maxAllowedWidth = width * (1 + tolerance)
				maxAllowedHeight = height * (1 + tolerance)

		return clusters

	def calculateDefaultDPI(self, provider: ProviderImages) -> typing.Optional[typing.Tuple[int, int]]:
		"""Calculate the DPI based on the most common page size."""

		dimensions: typing.List[typing.Tuple[int, int]] = []
		for index, path in enumerate(provider.images):
			with Image.open(path) as image:
				width, height = image.size
				dimensions.append((width, height))

		if len(dimensions) == 0:
			print("There are no images to process, cannot estimate DPI.")
			return None

		clusters = ImagesConverter.clusteringDimensions(dimensions, tolerance=0.05)
		clusters.sort(key=lambda x: len(x), reverse=True)
		mostCommonCluster = clusters[0]
		count = len(mostCommonCluster)
		width = int((mostCommonCluster[0][0] + mostCommonCluster[-1][0]) / 2)
		height = int((mostCommonCluster[0][1] + mostCommonCluster[-1][1]) / 2)

		if count < len(provider.images) / 2:
			print(
			    f"Warning: Most common dimension {width}x{height} only appears {count} times, which is less than half of the total images, ignoring."
			)
			return None

		pageType = {
		    "A4": (210, 297),
		    "A4 Lanscape": (297, 210),
		    "US Letter": (215.9, 279.4),
		    "US Letter Landscape": (279.4, 215.9),
		    "BD": (240, 320),
		    "BD Landscape": (320, 240),
		}
		dpis = []
		for name, (pageWidth, pageHeight) in pageType.items():
			dpis.append((abs((width / height) - (pageWidth / pageHeight)), name, width / (pageWidth / 25.4),
			             height / (pageHeight / 25.4)))
		dpis.sort(key=lambda x: x[0])
		dpiMatch = dpis[0]

		if dpiMatch[0] > 0.1:
			print(
			    f"Warning: The most common dimension {width}x{height} does not match any standard page size closely enough (error {dpiMatch[0]}), ignoring."
			)
			return None

		dpi = min(int(dpiMatch[2]), int(dpiMatch[3]))
		print(
		    f"Estimating page type '{dpiMatch[1]}' with DPI {dpi} from most common dimensions {width}x{height} ({count} times)."
		)
		return dpi, dpi

	def process(self, provider: ProviderImages,
	            directory: pathlib.Path) -> typing.List[typing.Tuple[ProviderImages, typing.Optional[FlowEnum]]]:

		images = []
		maybeEstimatedDPI = None
		for index, path in enumerate(provider.images):
			with Image.open(path) as image:
				image = image.convert("RGB")
				scale = self.scale or 1.0
				comments: typing.List[str] = []

				if self.maxDPI is not None:

					# On the first run, estimate the DPI.
					if index == 0:
						maybeEstimatedDPI = self.calculateDefaultDPI(provider=provider)

					# Get the DPI of this image.
					maybeDPI = image.info.get("dpi", None)
					if maybeDPI is not None and maybeEstimatedDPI is not None:
						if abs((maybeDPI[0] - maybeEstimatedDPI[0]) / maybeEstimatedDPI[0]) > 0.1 or \
                                                                                                                                                                                                                                                                                                      abs((maybeDPI[1] - maybeEstimatedDPI[1]) / maybeEstimatedDPI[1]) > 0.1:
							comments.append(
							    f"image DPI {int(maybeDPI[0])}x{int(maybeDPI[1])} != estimated DPI {int(maybeEstimatedDPI[0])}x{int(maybeEstimatedDPI[1])}"
							)
							maybeDPI = maybeEstimatedDPI
					elif maybeDPI is None:
						maybeDPI = maybeEstimatedDPI

					# Calculate new dimensions to achieve DPI.
					if maybeDPI is not None:
						comments.append(f"image DPI {int(maybeDPI[0])}x{int(maybeDPI[1])}")
						if maybeDPI[0] > self.maxDPI or maybeDPI[1] > self.maxDPI:
							scaleX = self.maxDPI / maybeDPI[0]
							scaleY = self.maxDPI / maybeDPI[1]
							scaleDPI = min(scaleX, scaleY)
							assert scaleDPI <= scale, f"Scaling by DPI must be less than or equal to {scale}, got {scaleDPI}."
							comments.append(f"matching max DPI of {self.maxDPI}")
							scale = scaleDPI

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
