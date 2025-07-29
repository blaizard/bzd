import pathlib
import typing
import statistics
import os
import re
from collections import Counter

from PIL import Image

from apps.ebook.flow import ActionInterface, FlowEnum
from apps.ebook.providers import ProviderEbook, ProviderImages


class Comics(ActionInterface):
	"""Convert a comics file into a sequence of images."""

	ProviderInput = ProviderEbook
	ProviderOutput = ProviderImages

	def __init__(self, coefficient: float = 1.0) -> None:
		super().__init__()
		self.coefficient = coefficient

	def generateNGrams(self, text: str, n: int) -> typing.List[str]:
		"""Generate the ngrams for a string."""

		ngrams = zip(*[text[i:] for i in range(n)])
		return ["".join(ngram) for ngram in ngrams]

	def outliers(self, images: typing.List[pathlib.Path],
	             coefficient: float) -> typing.Tuple[typing.List[pathlib.Path], typing.List[pathlib.Path]]:
		"""Identify outliers in the file name."""

		nGramsAll = {}

		nGramsCounter: Counter[str] = Counter()
		for image in images:
			# Ignore the actual numbers. This is needed otherwise a variation of 1 digit might make the name outlier.
			# What we want instead is to know that this is a number and test against it.
			stem = re.sub(r"\d+", "|", image.stem)
			nGrams = self.generateNGrams(stem, min(3, len(stem)))  #  minimum one entry.
			nGramsAll[image] = nGrams
			nGramsCounter.update(nGrams)
		nGramsCounterNormalized = {item: count / len(images) for item, count in nGramsCounter.items()}

		# Create a normalized score reflecting the commonality of the string vs the others.
		scores = {}
		for image, nGrams in nGramsAll.items():
			scores[image] = sum([nGramsCounterNormalized[gram] for gram in nGrams]) / len(nGrams)

		# Calculate the standard deviation and calculate the threshold to find the outliers
		if len(scores) >= 2:

			stdev = max(statistics.stdev(scores.values()), 0.1)
			while True:
				threshold = max(scores.values()) - stdev * coefficient
				scoresOutliers = {image: value for image, value in scores.items() if value < threshold}
				# If there are less than half of the images as outliers, we stop.
				# Else it probably means that the threshold was too high, then update it.
				if len(scoresOutliers) == 0 or len(scoresOutliers) < len(images) / 2:
					break
				stdev = max(scores.values()) - max(scoresOutliers.values())

			outliers = [*scoresOutliers.keys()]

		else:
			outliers = []

		return outliers, [image for image in images if not (image in outliers)]

	def processDirectory(
	        self, provider: ProviderEbook,
	        directory: pathlib.Path) -> typing.List[typing.Tuple[ProviderImages, typing.Optional[FlowEnum]]]:

		# Find all images per directory.
		allImagesByDirectory: typing.Dict[pathlib.Path, typing.List[pathlib.Path]] = {}
		for subdir, dirs, files in os.walk(directory):
			for file in files:
				# Check if this is a valid image.
				subdirectory = pathlib.Path(subdir)
				try:
					with Image.open(subdirectory / file) as img:
						img.verify()
				except (IOError, SyntaxError):
					continue
				allImagesByDirectory.setdefault(subdirectory, []).append(subdirectory / file)

		outputs: typing.List[typing.Tuple[ProviderImages, typing.Optional[FlowEnum]]] = []
		for subdirectory, allImages in allImagesByDirectory.items():

			# Sort all images.
			allImages.sort()

			# Check the outliers.
			outliers, allImages = self.outliers(allImages, coefficient=self.coefficient)
			for outlier in outliers:
				print(f"Ignoring {outlier} which is considered an outlier.")
			print(f"Identified {len(allImages)} valid images under {subdirectory}.")
			if len(allImages):
				outputs.append((ProviderImages(images=allImages, metadata=provider.metadata), None))

		return outputs
