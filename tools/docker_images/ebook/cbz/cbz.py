import pathlib
import typing
import zipfile
import statistics

from PIL import Image

from tools.docker_images.ebook.flow import ActionInterface, FlowEnum
from tools.docker_images.ebook.providers import ProviderEbook, ProviderImages


class Cbz(ActionInterface):
	"""Convert a .cbz file into a sequence of images."""

	ProviderInput = ProviderEbook
	ProviderOutput = ProviderImages

	def outliers(
	        self,
	        images: typing.List[pathlib.Path]) -> typing.Tuple[typing.List[pathlib.Path], typing.List[pathlib.Path]]:
		"""Identify outliers in the file name."""

		values: typing.List[int] = []
		for index, image in enumerate(images):
			name = image.stem
			values.append(len(name))

		mean = sum(values) / len(values)
		stdev = statistics.stdev(values)
		threshold = 2 * stdev

		outliers = []
		nonOutliers = []
		for index, value in enumerate(values):
			if abs(value - mean) > threshold:
				outliers.append(images[index])
			else:
				nonOutliers.append(images[index])

		return outliers, nonOutliers

	def process(self, provider: ProviderEbook,
	            directory: pathlib.Path) -> typing.List[typing.Tuple[ProviderImages, typing.Optional[FlowEnum]]]:

		with zipfile.ZipFile(str(provider.ebook), "r") as zipInstance:
			zipInstance.extractall(directory / "extract")

		# Find all images.
		allImages: typing.List[pathlib.Path] = []
		for f in (directory / "extract").iterdir():
			if not f.is_file():
				continue
			try:
				with Image.open(f) as img:
					img.verify()
			except (IOError, SyntaxError):
				continue
			allImages.append(f)
		allImages.sort()

		# Check the outliers.
		outliers, allImages = self.outliers(allImages)
		for outlier in outliers:
			print(f"Ignoring {outlier} which is considered an outlier.")
		print(f"Identified {len(allImages)} images.")

		return [(ProviderImages(images=allImages, metadata=provider.metadata), None)]
