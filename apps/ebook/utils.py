import math
import typing
from dataclasses import dataclass


def sizeToString(sizeBytes: int) -> str:
	"""Converts a size in bytes to a human-readable string (e.g., KB, MB, GB)."""

	if sizeBytes == 0:
		return "0 Bytes"

	# Define the units and their corresponding powers of 1024
	units = ["Bytes", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"]

	i = int(math.floor(math.log(sizeBytes, 1024)))
	p = math.pow(1024, i)
	size = round(sizeBytes / p, 2)

	return f"{size} {units[i]}"


def percentToString(ratio: float) -> str:
	"""Convert a ratio into a percentage."""

	return f"{(ratio * 100):.1f}%"


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


def estimatePageDPIs(dimensions: typing.List[typing.Tuple[int, int]]) -> typing.Optional[typing.List[float]]:
	"""Calculate the DPI of each pages based on the most common page size."""

	@dataclass
	class DPIPageEntry:
		"""Entry for matching a DPI."""

		name: str
		ratioDiff: float
		dpiX: float
		dpiY: float

	if len(dimensions) == 0:
		print("There are no images to process, cannot estimate DPI.")
		return None

	clusters = clusteringDimensions(dimensions, tolerance=0.05)
	clusters.sort(key=lambda x: len(x), reverse=True)
	mostCommonCluster = clusters[0]
	count = len(mostCommonCluster)
	width = (mostCommonCluster[0][0] + mostCommonCluster[-1][0]) / 2
	height = (mostCommonCluster[0][1] + mostCommonCluster[-1][1]) / 2

	if count < len(dimensions) / 2:
		print(
		    f"Warning: Most common dimension {int(width)}x{int(height)} only appears {count} times, which is less than half of the total images, ignoring."
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
	dpis: typing.List[DPIPageEntry] = []
	for name, (pageWidth, pageHeight) in pageType.items():
		dpis.append(
		    DPIPageEntry(name=name,
		                 ratioDiff=abs((width / height) - (pageWidth / pageHeight)),
		                 dpiX=width / (pageWidth / 25.4),
		                 dpiY=height / (pageHeight / 25.4)))
	dpis.sort(key=lambda x: x.ratioDiff)
	dpiMatch = dpis[0]

	if dpiMatch.ratioDiff > 0.1:
		print(
		    f"Warning: The most common dimension {int(width)}x{int(height)} does not match any standard page size closely enough (error {dpiMatch.ratioDiff}), ignoring."
		)
		return None

	dpi = min(dpiMatch.dpiX, dpiMatch.dpiY)
	print(
	    f"Estimating page type '{dpiMatch.name}' with DPI {int(dpi)} from most common dimensions {int(width)}x{int(height)} ({count} times)."
	)

	# Check every pages, identify outliers and estimate the DPI per pages.
	dpiPerPage: typing.List[float] = []
	area = width * height
	for index, (pageWidth, pageHeight) in enumerate(dimensions):
		pageArea = pageWidth * pageHeight

		# Area should be within ~2x larger or most common page. This accounts to some pages that are double pages,
		# anything larger is likely an outlier.
		if abs((pageArea - area) / area) > 2.1:
			pageDPI = dpi * math.sqrt(pageArea / area)
			print(
			    f"Page {index} is likely an outlier: {int(pageWidth)}x{int(pageHeight)}, esitimated DPI {int(pageDPI)}."
			)
			dpiPerPage.append(pageDPI)
		else:
			dpiPerPage.append(dpi)

	return dpiPerPage
